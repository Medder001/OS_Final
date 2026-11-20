#include "O5.h"
#include "io.h"

#define SBI_MAGIC 0x08050000
#define SBI_MASK  0xFFFF0000

o5ret_t o5_ecall(i32 a0, i32 a1, i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7);

o5ret_t o5_call(i32 a0, i32 a1, i32 a2, i32 a3, i32 a4, i32 a5, i32 a6, i32 a7) {
    return o5_ecall(a0, a1, a2, a3, a4, a5, a6, a7 | SBI_MAGIC);
}

void o5_fini() {
    o5_call(0, 0, 0, 0, 0, 0, 0, O5_CALL_FINI);
}

o5ret_t o5_read(u32 port, u32 num_bytes, u8 *ptr_bytes) {
    return o5_call((i32) port, (i32) num_bytes, (i32) ptr_bytes, 0, 0, 0, 0, O5_CALL_READ);
}

o5ret_t o5_write(u32 port, u32 num_bytes, u8 *ptr_bytes) {
    return o5_call( (i32) port, (i32) num_bytes, (i32) ptr_bytes, 0, 0, 0, 0, O5_CALL_WRITE);
}

o5ret_t o5_info() {
    o5_call(0, 0, 0, 0, 0, 0, 0, O5_CALL_INFO);
}