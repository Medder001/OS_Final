#pragma once

#include "common.h"
#include "strings.h"

#define IO_SUCCESS   0
#define IO_ERROR    -1
#define IO_EMPTY    -2

i32 write_chars(char *cs);
i32 read_char(char *ptr_c);
i32 readln_poll(char *buf, u32 len);

i32 write_bstr(BStr *bstr);