# 
#
#

	.global _start
	.section .text

_start:
    la t0, kernel_id_hart   # store Hart ID
    sw a0, 0(t0)            #   in C-code variable
    la t0, kernel_ptr_dtb   # store DTB address
    sw a1, 0(t0)            #   in C-code variable

	li sp, 0x80400000	    # isr stack at 4MB from base
	csrw sscratch, sp	    #   into supervisor scratch reg
	li sp, 0x80800000	    # kernel stack at 8MB from base
	call kernel_init

loop:
	j loop


	.global sbi_ecall
sbi_ecall:
	ecall
	ret	


	.global interrupts_enable
interrupts_enable:
	la a0, trap_vector
	la a1, 0b01	            # mask MODE=Vectored
	or a0, a0, a1
	csrw stvec, a0		    # set trap vector base address
	csrsi sstatus, 0b10	    # set interrupts enabled
	ret


    .align 4
trap_vector:
	j handle_exception	        # trap_vector[ 0]
	sret			            # trap_vector[ 1]
	sret 			            # trap_vector[ 2]
	sret 			            # trap_vector[ 3]
	sret 			            # trap_vector[ 4]
	j handle_timer_interrupt    # trap_vector[ 5]
	sret			            # trap_vector[ 6]
	sret			            # trap_vector[ 7]
	sret			            # trap_vector[ 8]
	sret			            # trap_vector[ 9]
	sret			            # trap_vector[10]
	sret 			            # trap_vector[11]
	sret 			            # trap_vector[12]
	sret			            # trap_vector[13]
	sret			            # trap_vector[14]
	sret			            # trap_vector[15]


handle_exception:

	# atomic swap sp and supervisor scratch
	csrrw sp, sscratch, sp

	# store registers on isr stack
	addi sp, sp, -32
	sw ra,  0(sp)
    sw t0,  4(sp)
	sw t1,  8(sp)
    sw t2, 12(sp)
    sw t3, 16(sp)
	sw t4, 20(sp)
	sw t5, 24(sp)
	sw t6, 28(sp)

    # check for ecall from U-mode
	li t0, 8		    # exception code 8: ecall from U-mode
	csrr t1, scause		# get scause CSR
	beq t0, t1, handle_exception_umode_ecall

	# dont' handle other types of exceptions
	j handle_exception_done

    handle_exception_umode_ecall:

    # check for O5 Call with MAGIC
    li t0, 0x08050000   # SBI_MAGIC
    li t1, 0xFFFF0000   # SBI_MASK
    and t1, t1, a7      # SCALL MAGIC
    bne t0, t1, handle_exception_pass_through
    li t1, 0x0000FFFF   # inverted SBI_MASK
    and a7, t1, a7      # remove magic
    j handle_exception_o5_call

    # pass-through certain SBI calls
    #   and don't handle other U-mode ecalls
    handle_exception_pass_through:

    li t0, 0x4442434E   # SBI EID 'DBCN'
    bne t0, a7, handle_exception_done
	li t0, 0x53525354	# SBI EID 'SRST'
	beq t0, a7, handle_exception_done

    # assume SBI arguments are in a0-a7
    handle_exception_mcall_sbi:
	ecall # trap into M-mode
	j handle_exception_done

    # handle O5 System Calls
    handle_exception_o5_call:

	# store registers in isr stack
	addi sp, sp, -64
	sw a0, 0(sp)
	sw a1, 4(sp)
	sw a2, 8(sp)
	sw a3, 12(sp)
	sw a4, 16(sp)
	sw a5, 20(sp)
	sw a6, 24(sp)
	sw a7, 28(sp)

    1:  # SCALL_FINI [0x01]
        li t0, 0x01
        bne t0, a7, 1f
        call kernel_scall_fini
        j handle_exception_scall_done
    1:  # SCALL_READ [0x1A]
        li t0, 0x1A
        bne t0, a7, 1f
        call kernel_scall_read
        j handle_exception_scall_done
    1:  # SCALL_WRITE [0x1B]
        li t0, 0x1B
        bne t0, a7, 1f
        call kernel_scall_write
        j handle_exception_scall_done
    1:  # SCALL_INFO [0x2A]
        li t0, 0x2A
        bne t0, a7, 1f
        call kernel_scall_info
        j handle_exception_scall_done
    1:  # End-of-O5_Calls

    handle_exception_scall_done:

#    lw a0, 0(sp)
#    lw a1, 4(sp)
    lw a2, 8(sp)
    lw a3, 12(sp)
    lw a4, 16(sp)
    lw a5, 20(sp)
    lw a6, 24(sp)
    lw a7, 28(sp)
	addi sp, sp, +64

	j handle_exception_done

    handle_exception_done:

	# increase sepc
	csrr t0, sepc		# get SEPC
	addi t0, t0, 4		# increase SEPC by 4
	csrw sepc, t0		# set modified SEPC

	# load registers from isr stack
	lw ra,  0(sp)
    lw t0,  4(sp)
	lw t1,  8(sp)
    lw t2, 12(sp)
    lw t3, 16(sp)
	lw t4, 20(sp)
	lw t5, 24(sp)
	lw t6, 28(sp)
	addi sp, sp, +32

	# atomic swap sp and supervisor scratch
	csrrw sp, sscratch, sp

	# return from s-mode interrupt
	sret


handle_timer_interrupt:

    # atomic swap sp and supervisor scratch
	csrrw sp, sscratch, sp

	# move stack pointer
	addi sp, sp, -128
	# store all 31 4-byte registers (124 bytes)
    sw  x1,   4(sp)  #  ra, return address
    #  ------------     sp, stack pointer
    sw  x3,  12(sp)  #  gp, global pointer
    sw  x4,  16(sp)  #  tp, thread pointer
    sw  x5,  20(sp)  #  t0, temp 0
    sw  x6,  24(sp)  #  t1, temp 1
    sw  x7,  28(sp)  #  t2, temp 2
    sw  x8,  32(sp)  #  s0, saved  0 / frame pointer
    sw  x9,  36(sp)  #  s1, saved  1
    sw x10,  40(sp)  #  a0, arg 0 / return 0
    sw x11,  44(sp)  #  a1, arg 1 / return 1
    sw x12,  48(sp)  #  a2, arg 2
    sw x13,  52(sp)  #  a3, arg 3
    sw x14,  56(sp)  #  a4, arg 4
    sw x15,  60(sp)  #  a5, arg 5
    sw x16,  64(sp)  #  a6, arg 6
    sw x17,  68(sp)  #  a7, arg 7
    sw x18,  72(sp)  #  s2, saved  2
    sw x19,  76(sp)  #  s3, saved  3
    sw x20,  80(sp)  #  s4, saved  4
    sw x21,  84(sp)  #  s5, saved  5
    sw x22,  88(sp)  #  s6, saved  6
    sw x23,  92(sp)  #  s7, saved  7
    sw x24,  96(sp)  #  s8, saved  8
    sw x25, 100(sp)  #  s9, saved  9
    sw x26, 104(sp)  # s10, saved 10
    sw x27, 108(sp)  # s11, saved 11
    sw x28, 112(sp)  #  t3, temp 3
    sw x29, 116(sp)  #  t4, temp 4
    sw x30, 120(sp)  #  t5, temp 5
    sw x31, 124(sp)  #  t6, temp 6

	# clear pending interrupt
	li a0, 0b100000
	csrc sip, a0	# clear bit 5 (STIP) in supervisor interrupts pending (SIP)
	csrc sie, a0	# clear bit 5 (STIE) in supervisor interrupts enabled (SIE)
	
	# SBI set time_out to far future
	# li a7, 0x54494D45       # SBI EID 'TIME'
    # li a6, 0                # SBI FID 0 (sbi_set_timer)
    # li a0, 0x7FFFFFFF
	# li a1, 0x7FFFFFFF
	# ecall

	call kernel_timeout
	
    # DO NOT INCREASE SEPC
    csrr t0, sepc    # get SEPC
    addi t0, t0, 4   # increase SEPC by 4
    csrw sepc, t0    # set modified SEPC

	# load all 31 4-byte registers (124 bytes)
    lw  x1,   4(sp)  #  ra, return address
    #  ------------     sp, stack pointer
    lw  x3,  12(sp)  #  gp, global pointer
    lw  x4,  16(sp)  #  tp, thread pointer
    lw  x5,  20(sp)  #  t0, temp 0
    lw  x6,  24(sp)  #  t1, temp 1
    lw  x7,  28(sp)  #  t2, temp 2
    lw  x8,  32(sp)  #  s0, saved  0 / frame pointer
    lw  x9,  36(sp)  #  s1, saved  1
    lw x10,  40(sp)  #  a0, arg 0 / return 0
    lw x11,  44(sp)  #  a1, arg 1 / return 1
    lw x12,  48(sp)  #  a2, arg 2
    lw x13,  52(sp)  #  a3, arg 3
    lw x14,  56(sp)  #  a4, arg 4
    lw x15,  60(sp)  #  a5, arg 5
    lw x16,  64(sp)  #  a6, arg 6
    lw x17,  68(sp)  #  a7, arg 7
    lw x18,  72(sp)  #  s2, saved  2
    lw x19,  76(sp)  #  s3, saved  3
    lw x20,  80(sp)  #  s4, saved  4
    lw x21,  84(sp)  #  s5, saved  5
    lw x22,  88(sp)  #  s6, saved  6
    lw x23,  92(sp)  #  s7, saved  7
    lw x24,  96(sp)  #  s8, saved  8
    lw x25, 100(sp)  #  s9, saved  9
    lw x26, 104(sp)  # s10, saved 10
    lw x27, 108(sp)  # s11, saved 11
    lw x28, 112(sp)  #  t3, temp 3
    lw x29, 116(sp)  #  t4, temp 4
    lw x30, 120(sp)  #  t5, temp 5
    lw x31, 124(sp)  #  t6, temp 6
    # move stack pointer
	addi sp, sp, +128

	# atomic swap sp and supervisor scratch
	csrrw sp, sscratch, sp

	# return from s-mode interrupt
	sret


	.global interrupts_timer_enable
interrupts_timer_enable:
	li a0, 0b00100000	# bit mask
	csrs sie, a0		# set bit 5 (SIE.STIE)
	ret


	.global timer_get
timer_get:
	rdtimeh a2
	rdtime a0
	rdtimeh a1
	bne a1, a2, timer_get
	ret


# relevant bits/bitfields:
#
#   sstatus.SIE - Supervisor Interrupts Enabled
#   sstatus.SPP - Supervisor Previous Priviledge
#   sstatus.SPIE - Supervisor Prior Interrupts Enabled
#
# execution of sret instruction:
#
#   mode = sstatus.SPP  // switch priveledge mode to S-mode Previos Priviledge
#   hstatus.SPV = 0     // ...
#   sstatus.SPP = 0     // set S-mode Previos Priviledge bit to 0
#   sstatus.SIE = SPIE  // set S-mode Interrupts Enabled to previous setting
#   sstatus.SPIE = 1    // set S-mode Previous Interrupts Enabled bit to 1 
#   pc=sepc             // set program counter
#
# see: 3.1.6.1. Privilege and Global Interrupt-Enable Stack in mstatus register (Part II)

#
	.global user_init
user_init:
	li sp, 0x81000000	# set user stack at 16MB from base
				        # expect address in a0
	li t0, 0x00000100	# sstatus.SPP is bit 8 
	csrc sstatus, t0 	# sstatus.SPP = 0 (next mode: U-mode)
	
	# la contains function pointer
    csrw sepc, a0	    # set Supervisor Exception Program Counter
    sret			    # never return


	.global o5_ecall
o5_ecall:
	ecall			    # call into S-mode (trap)
    ret 			    # return to U-mode code


#    .section .rodata
#timer_msg:
#    .string "TIMER\n"
#	timer_msg_len = (. - timer_msg)


#    .section .rodata
#scall_msg:
#    .string "SCALL\n"
#	scall_msg_len = (. - scall_msg)

