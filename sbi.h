#pragma once

#include "common.h"


#define SBI_SUCCESS 0
#define SBI_ERR_FAILED -1
#define SBI_ERR_NOT_SUPPORTED -2
#define SBI_ERR_INVALID_PARAM -3
#define SBI_ERR_DENIED -4
#define SBI_ERR_INVALID_ADDRESS -5
#define SBI_ERR_ALREADY_AVAILABLE -6
#define SBI_ERR_ALREADY_STARTED -7
#define SBI_ERR_ALREADY_STOPPED -8
#define SBI_ERR_NO_SHMEM -9

#define SBI_SRST_TYPE_SHUTDOWN 0
#define SBI_SRST_REASON_NONE 0

struct sbiret {
    isize error;
    isize value;
};
typedef struct sbiret sbiret_t;

sbiret_t sbi_ecall(
	u32 p0,
	u32 p1,
	u32 p2,
	u32 p3,
	u32 p4,
	u32 p5,
	u32 fid,
	u32 eid);

sbiret_t sbi_debug_console_read(u32 num_bytes, u32 base_addr_lo, u32 base_addr_hi);
sbiret_t sbi_debug_console_write(u32 num_bytes, u32 base_addr_lo, u32 base_addr_hi);
sbiret_t sbi_debug_console_write_byte(u8 byte);

sbiret_t sbi_system_reset(u32 reset_type, u32 reset_reason);

sbiret_t sbi_set_timer(u64 stime_value);

