#pragma once

#include "common.h"

#define FMT_SHOW_BASE   0x01000000
#define FMT_SHOW_SEP    0x02000000
#define FMT_SHOW_SIGN   0x04000000
#define FMT_UNSIGNED    0x08000000

#define FMT_BIN     0x00000001
#define FMT_QAT     0x00000003
#define FMT_OCT     0x00000007
#define FMT_DEC     0x00000009
#define FMT_HEX     0x0000000F

#define FMT_SEP2    0x00000010
#define FMT_SEP3    0x00000020
#define FMT_SEP4    0x00000030
#define FMT_SEP8    0x00000070
#define FMT_SEP16   0x000000F0

#define FMT_SHOW_BIN (FMT_SHOW_BASE | FMT_BIN)
#define FMT_SHOW_QAT (FMT_SHOW_BASE | FMT_QAT)
#define FMT_SHOW_OCT (FMT_SHOW_BASE | FMT_OCT)
#define FMT_SHOW_DEC (FMT_SHOW_BASE | FMT_DEC)
#define FMT_SHOW_HEX (FMT_SHOW_BASE | FMT_HEX)

#define FMT_SHOW_SEP2 (FMT_SHOW_SEP | FMT_SEP2)
#define FMT_SHOW_SEP3 (FMT_SHOW_SEP | FMT_SEP3)
#define FMT_SHOW_SEP4 (FMT_SHOW_SEP | FMT_SEP4)
#define FMT_SHOW_SEP8 (FMT_SHOW_SEP | FMT_SEP8)
#define FMT_SHOW_SEP16 (FMT_SHOW_SEP | FMT_SEP16)


isize chars_len(char *cs);
isize chars_reverse(char *cs);

i32 str_nt_eq(char *a, char *b);

void u64_2_str_0x(u64 value, u8 u8x22[22]);
isize i32_to_chars(i32 value, char *cs, usize cap);
isize i32_to_chars_fmt(i32 value, char *cs, usize cap, usize fmt);
isize u32_to_chars_fmt(u32 value, char *cs, usize cap, usize fmt);

// Byte Strings (BStr)

typedef struct {
    usize cap;
    usize len;
    u8 *bytes;
} BStr;

void BStr_init(BStr *bstr, usize cap, usize len, u8 *bytes);
void BStr_copy_chars(BStr *bstr, char *cs);
void BStr_reverse(BStr *bstr);

// holds 60 = 64-4 bytes
typedef struct {
    BStr bstr;
    u8 bytes[61];
} BStrBuf64;

BStr * BStrBuf64_init(BStrBuf64 *buf);
BStr * BStrBuf64_init_chars(BStrBuf64 *buf, char *chars);
BStr* BStrBuf64_bstr(BStrBuf64 *buf);
