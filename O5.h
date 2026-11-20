#pragma once

#include "common.h"

#define O5_SUCCESS 0

#define O5_IN	0
#define O5_OUT	1

#define O5_CALL_FINI      0x01
#define O5_CALL_READ      0x1A
#define O5_CALL_WRITE     0x1B
#define O5_CALL_INFO      0x2A

struct o5ret {
    isize status;
    isize value;
};
typedef struct o5ret o5ret_t;

void o5_fini();
o5ret_t o5_read(u32 port, u32 num_bytes, u8 *ptr_bytes);
o5ret_t o5_write(u32 port, u32 num_bytes, u8 *ptr_bytes);
o5ret_t o5_info();