#include "kernel.h"
#include "sbi.h"
#include "strings.h"
#include "shell.h"
#include "o5.h"

usize kernel_id_hart = 0;
usize kernel_ptr_dtb = 0;

void interrupts_enable();
void interrupts_timer_enable();
u64 timer_get();

void user_init(void *func);
void program_frame();


void kernel_msg(char *str_nt) {
    isize len = chars_len(str_nt);
    sbi_debug_console_write(len, (u32) str_nt, 0);
}

void kernel_msg_bstr(BStr *bstr) {
    sbi_debug_console_write(bstr->len, (u32) bstr->bytes, 0);
}


void kernel_init_msg() {
	kernel_msg("\n");
	kernel_msg(O5_NAME " '" O5_EDITION "'");
	kernel_msg(" (" O5_VERSION ")\n");
	kernel_msg("\n");

    const isize cbuf_len = 256;
    char cbuf[cbuf_len];
    kernel_msg("  id_hart = ");
    i32_to_chars(kernel_id_hart, cbuf, cbuf_len);
    kernel_msg(cbuf);
    kernel_msg("\n");

    kernel_msg("  ptr_dtb = ");
    u32_to_chars_fmt(kernel_ptr_dtb, cbuf, cbuf_len, FMT_SHOW_HEX | FMT_SHOW_SEP4 | FMT_SHOW_SIGN );
    kernel_msg(cbuf);
    kernel_msg("\n");
    kernel_msg("\n");
}


void kernel_init_interrupts() {

    kernel_msg("enable interrupts\n");
    interrupts_enable();

    u64 dt = 0x00100000; // about a second
    u64 t0 = timer_get();
    u64 t1 = t0 + 3*dt;

    sbi_set_timer(t1);
    // interrupts_timer_enable();
}


// note: executed in USER MODE
void user_init_frame() {
    user_shell();
    o5_fini();

    // no-return
}


void kernel_init() {

    kernel_init_msg();
    kernel_init_interrupts();

    user_init((void *) user_init_frame);

    // no-return
}

u64 kernel_timeout_n = 0;

void kernel_timeout() {
    // kernel_msg("\n[kernel:timeout]\n");
    kernel_timeout_n += 1;

    u64 dt = 0x00100000; // about a second
    u64 t0 = timer_get();
    u64 t1 = t0 + 3*dt;

    sbi_set_timer(t1);
    interrupts_timer_enable();
}


o5ret_t kernel_scall_read(u32 port, u32 num_bytes, u8 *ptr_bytes) {
    // kernel_msg("[kernel_scall_read]\n");
    sbiret_t sbiret = sbi_debug_console_read(num_bytes, (u32) ptr_bytes, 0);
    o5ret_t ret = { .status = sbiret.error, .value = sbiret.value };
    return ret;
}

o5ret_t kernel_scall_write(u32 port, u32 num_bytes, u8 *ptr_bytes) {
    // kernel_msg("[kernel:scall_write]\n");
    sbi_debug_console_write(num_bytes, (u32) ptr_bytes, 0);
    o5ret_t ret = { .status = O5_SUCCESS, .value = 42 };
    return ret;
}

o5ret_t kernel_scall_info() {
    kernel_msg("[kernel:scall_info]\n");
    kernel_msg("  Note: We are in S-mode now.\n");
    kernel_msg("  \n");
    o5ret_t ret = { .status = O5_SUCCESS, .value = 0 };
    return ret;
}

void kernel_scall_fini() {
    kernel_msg("[kernel:scall_fini] SHUTDOWN\n");
    sbi_system_reset(SBI_SRST_TYPE_SHUTDOWN, SBI_SRST_REASON_NONE);
    // no-return
}

