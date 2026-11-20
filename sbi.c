#include "sbi.h"

#define SBI_DBCN_EID 0x4442434E  // 'DBCN'
#define SBI_DBCN_FID_CONSOLE_WRITE 0
#define SBI_DBCN_FID_CONSOLE_READ  1
#define SBI_DBCN_FID_CONSOLE_WRITE_BYTE 2

#define SBI_SRST_EID 0x53525354 // 'SRST'
#define SBI_SRST_FID_SYSTEM_RESET 0

#define SBI_TIME_EID 0x54494D45 // 'TIME'
#define SBI_TIME_FID_SET_TIMER 0

sbiret_t sbi_debug_console_read(u32 num_bytes, u32 base_addr_lo, u32 base_addr_hi)
{
	return sbi_ecall(
		num_bytes, (usize) base_addr_lo, base_addr_hi,
		0, 0, 0,
		SBI_DBCN_FID_CONSOLE_READ,
		SBI_DBCN_EID);
}

sbiret_t sbi_debug_console_write(u32 num_bytes, u32 base_addr_lo, u32 base_addr_hi)
{
    return sbi_ecall(
		num_bytes, (usize) base_addr_lo, base_addr_hi,
		0, 0, 0,
        SBI_DBCN_FID_CONSOLE_WRITE,
        SBI_DBCN_EID);
}

sbiret_t sbi_debug_console_write_byte(u8 byte)
{
	return sbi_ecall(
		(usize) byte,
		0, 0, 0, 0, 0,
		SBI_DBCN_FID_CONSOLE_WRITE_BYTE,
		SBI_DBCN_EID);
}

sbiret_t sbi_system_reset(u32 reset_type, u32 reset_reason)
{
    return sbi_ecall(
		reset_type, reset_reason,
		0, 0, 0, 0,
		SBI_SRST_FID_SYSTEM_RESET,
		SBI_SRST_EID);
}

sbiret_t sbi_set_timer(u64 stime_value)
{
    return sbi_ecall(
        (u32) (stime_value),
		(u32) (stime_value >> 32),
		0, 0, 0, 0,
		SBI_TIME_FID_SET_TIMER,
		SBI_TIME_EID);
}


