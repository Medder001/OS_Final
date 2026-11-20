#include "strings.h"

isize chars_len(char *cs) {
	if (cs == 0)
		return -1;
    isize len = 0;
    while (cs[len] != '\0') {
        len += 1;
    }
    return len;
}

isize chars_reverse(char *cs) {
	if (cs == 0)
		return -1;
	isize len = 0;
	while (cs[len] != '\0') {
		len += 1;
	}
	if (len < 2)
		return len;
	isize half = len / 2;
	isize end = len - 1;
	for (isize i=0; i<half; i+=1) {
		char c = cs[i];
		cs[i] = cs[end-i];
		cs[end-i] = c;
	}
}

i32 str_nt_eq(char *a, char *b) {
    u32 end;

    while (a[0] != '\0' || b[0] != '\0') {
        if (a[0] == '\0' || b[0] == '\0') {
            return 0;
        }
        if (a[0] != b[0]) {
            return 0;
        }
        a += 1;
        b += 1;
    }
    return 1;
}


char syms_hex[16] = {
	'0','1','2','3',
	'4','5','6','7',
	'8','9','A','B',
	'C','D','E','F',
};

isize fmt_to_base(usize fmt) {
	usize fmt_base = 0x0000000F & fmt; // bits 0-3
	if (fmt_base == 0)
		return -3; // invalid base format
	return (isize) (fmt_base + 1);
}

char fmt_to_base_prefix(usize fmt) {
	usize fmt_base = 0x0000000F & fmt; // bits 0-3
	switch (fmt_base) {
		case 0x9: return 'd';
		case 0xF: return 'x';
		case 0x7: return 'o';
		case 0x3: return 'q';
		case 0x1: return 'b';
		default:
			return 0; // no prefix for given base
	}
}

isize fmt_to_sep_n(usize fmt) {
	usize fmt_sep_n = 0x000000F0 & fmt; // bits 4-7
	fmt_sep_n >>= 4;
	return (isize) (fmt_sep_n + 1);
}

char fmt_to_sep_char(usize fmt) {
	usize fmt_sep = 0x00FF0000 & fmt; // bits 16-23
	if (fmt_sep == 0)
		return '_';
	fmt_sep >>= 16;
	return (char) fmt_sep;
}

isize i32_to_chars_fmt(i32 value, char *cs, usize cap, usize fmt) {

	// extract base format
	isize base = fmt_to_base(fmt);
	if (base <= 0)
		return -3; // error: invalid base
	usize base_show = FMT_SHOW_BASE & fmt;
	char base_prefix = fmt_to_base_prefix(fmt);

	// extract separator format
	isize sep_show = FMT_SHOW_SEP & fmt;
	isize sep_n = fmt_to_sep_n(fmt);
	char sep_char = fmt_to_sep_char(fmt);

	// extract sign format
	usize sign_show = FMT_SHOW_SIGN & fmt;

	// buffer avail
	if (cap == 0)
		return -1; // error: buffer to small
	isize avail = (isize) cap;
	isize i = 0; // buffer index

	// orientation and magnitude of number
	isize is_neg = value < 0;
	isize magnitude = is_neg ? -value : value;

	// zero is different
	if (magnitude == 0) {
		if (avail-i < 1)
			return -1; // error: buffer to small
		cs[i] = '0';
		i += 1;
	}

	// if magnitude is non-zero
	while (magnitude > 0) {
		if (avail-i < 1)
			goto buffer_full;
		if (sep_show && i % (sep_n+1) == sep_n) {
			cs[i] = sep_char;
		} else {
			char digit = syms_hex[magnitude%base];
			magnitude = magnitude / base;
			cs[i] = digit;
		}
		i += 1;
	}

	// if base prefix should be shown
	if (base_show && base_prefix != 0) {
		if (sep_show) {
			if (avail-i < 1)
				goto buffer_full;
			cs[i] = sep_char;
			i += 1;
		}
		if (avail-i < 2)
			goto buffer_full;
		cs[i+0] = base_prefix;
		cs[i+1] = '0';
		i += 2;
	}

	// if value is negative or sign shown
	if (is_neg || sign_show) {
		if (avail-i < 1)
			goto buffer_full;
		cs[i] = is_neg ? '-' : '+';
		i += 1;
	}

	// null-terminate
	if (avail-i < 1)
		goto buffer_full;
	cs[i] = '\0';

	return chars_reverse(cs);

buffer_full:

	cs[i] = '\0'; // null-terminate
	return -1; // error: buffer to small
}

isize i32_to_chars(i32 value, char *cs, usize cap) {
	return i32_to_chars_fmt(value, cs, cap, FMT_DEC);
}

isize u32_to_chars_fmt(u32 value, char *cs, usize cap, usize fmt) {

	// extract base format
	isize base = fmt_to_base(fmt);
	if (base <= 0)
		return -3; // error: invalid base
	usize base_show = FMT_SHOW_BASE & fmt;
	char base_prefix = fmt_to_base_prefix(fmt);

	// extract separator format
	isize sep_show = FMT_SHOW_SEP & fmt;
	isize sep_n = fmt_to_sep_n(fmt);
	char sep_char = fmt_to_sep_char(fmt);

	// buffer avail
	if (cap == 0)
		return -1; // error: buffer to small
	isize avail = (isize) cap;
	isize i = 0; // buffer index

	// orientation and magnitude of number
	usize magnitude = value;

	// zero is different
	if (magnitude == 0) {
		if (avail-i < 1)
			return -1; // error: buffer to small
		cs[i] = '0';
		i += 1;
	}

	// if magnitude is non-zero
	while (magnitude > 0) {
		if (avail-i < 1)
			goto buffer_full;
		if (sep_show && i % (sep_n+1) == sep_n) {
			cs[i] = sep_char;
		} else {
			char digit = syms_hex[magnitude%base];
			magnitude = magnitude / base;
			cs[i] = digit;
		}
		i += 1;
	}

	// if base prefix should be shown
	if (base_show && base_prefix != 0) {
		if (sep_show) {
			if (avail-i < 1)
				goto buffer_full;
			cs[i] = sep_char;
			i += 1;
		}
		if (avail-i < 2)
			goto buffer_full;
		cs[i+0] = base_prefix;
		cs[i+1] = '0';
		i += 2;
	}

	// null-terminate
	if (avail-i < 1)
		goto buffer_full;
	cs[i] = '\0';

	return chars_reverse(cs);

buffer_full:

	cs[i] = '\0'; // null-terminate
	return -1; // error: buffer to small
}

isize u32_to_chars(u32 value, char *cs, usize cap) {
	return u32_to_chars_fmt(value, cs, cap, FMT_DEC);
}

// Bytes Strings (BStr)


void BStr_init(BStr *bstr, usize cap, usize len, u8 *bytes) {
	bstr->cap = cap;
	bstr->len = len;
	bstr->bytes = bytes;
}

void BStr_copy_chars(BStr *bstr, char *cs) {
	if (bstr == 0 || cs == 0)
		return;
	usize i = 0;
	while (cs[i] != '\0' && i < bstr->cap) {
		bstr->bytes[i] = cs[i];
		i += 1;
	}
	bstr->len = i;
	if (i < bstr->cap) {
		bstr->bytes[i] = '\0';
	}
}

void BStr_reverse(BStr *bstr) {
	if (bstr == 0)
		return;
	if (bstr->len < 2)
		return;
	isize half = bstr->len / 2;
	isize end = bstr->len - 1;
	for (isize i=0; i<half; i+=1) {
		char c = bstr->bytes[i];
		bstr->bytes[i] = bstr->bytes[end-i];
		bstr->bytes[end-i] = c;
	}
}


BStr * BStrBuf64_init(BStrBuf64 *buf) {
	if (buf == 0)
		return 0;
	buf->bytes[63] = '\0';
	BStr_init(BStrBuf64_bstr(buf), 60, 0, buf->bytes);
	return BStrBuf64_bstr(buf);
}

BStr * BStrBuf64_init_chars(BStrBuf64 *buf, char *chars) {
    BStrBuf64_init(buf);
	BStr_copy_chars(BStrBuf64_bstr(buf), chars);
	return BStrBuf64_bstr(buf);
}

BStr* BStrBuf64_bstr(BStrBuf64 *buf) {
	return &buf->bstr;
}

// HACK: necessary because GCC compiler generates implicit calls to memcpy!
#define size_t isize
void* memcpy(void* dst, const void* src, size_t n) {
	char* d = (char*) dst;
	const char* s = (const char*) src;
	while (n > 0) {
		*d++ = *s++;
		n -= 1;
	}
	return dst;
}

