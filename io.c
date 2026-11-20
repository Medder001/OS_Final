#include "io.h"

#include "strings.h"
#include "O5.h"

i32 write_chars(char *cs) {
    isize len = chars_len(cs);
    o5ret_t ret = o5_write(O5_IN, len, cs);
	return ret.value;
}

i32 read_char(char *ptr_c) {
	o5ret_t ret;
	ret = o5_read(O5_OUT, 1, ptr_c);
	if (ret.status != O5_SUCCESS) {
		return IO_ERROR;
	} else if (ret.value <= 0) {
		return IO_EMPTY;
	}
	return IO_SUCCESS;
}

void write_char(char c) {
	char char_nt[2] = { c, '\0' };
	write_chars(char_nt);
}


i32 read_char_poll(char *ptr_c) {
    i32 ret;
    do {
        ret = read_char(ptr_c);
    } while (ret == IO_EMPTY);
    return ret;
}


i32 write_bstr(BStr *bstr) {
	o5ret_t ret = o5_write(O5_IN, bstr->len, bstr->bytes);
	return ret.value;
}


#define ANSI_LEFT  "\x1B[D"
#define ANSI_RIGHT "\x1B[C"
#define ANSI_UP    "\x1B[A"
#define ANSI_DOWN  "\x1B[B"

#define KEY_ESC   0x0027
#define KEY_LEFT  0x2190
#define KEY_RIGHT 0x2192
#define KEY_UP    0x2191
#define KEY_DOWN  0x2193

i32 parse_esc(u32 *code) {
    char c;
    char state = 'E';
    i32 ret;

    ret = read_char(&c);
    if (ret == IO_ERROR) {
        return IO_ERROR;
    } else if (ret == IO_EMPTY) {
    	*code = KEY_ESC;
    	return IO_SUCCESS;
    }

    while (1) {
        switch (state) {
			case 'E':
				switch (c) {
					case '[': state = '['; break;
					default: return IO_ERROR;
				}
				break;
	        case '[':
				switch (c) {
				    case 'D': *code = KEY_LEFT; return IO_SUCCESS;
				    case 'C': *code = KEY_RIGHT; return IO_SUCCESS;
				    case 'A': *code = KEY_UP; return IO_SUCCESS;
				    case 'B': *code = KEY_DOWN; return IO_SUCCESS;
				    default: return IO_ERROR; // could not parse
				}
				break;
	        default:
	            return IO_ERROR;
        }
    	ret = read_char_poll(&c);
    	if (ret == IO_ERROR) {
    		return IO_ERROR;
    	}
    }
}

i32 readln_poll(char *line, u32 len) {
    if (len < 2) {
        return -1;
    }
    i32 error;
    u32 pos = 0;
    char c;
    while (1) {
        error = read_char_poll(&c);
        if (error != IO_SUCCESS) {
			return error;
		}

		// if 'carriage return' (13, '\r')
		if (c == '\r') {
		    line[pos] = '\0';
		    write_chars("\r\n");
		    return 0;
		} else {
		    // if ESC (27) code sequence
		    if (c == 0x1B) {
		        // parse ESC
				u32 key;
				if (!parse_esc(&key)) {
					switch(key) {
					case KEY_LEFT: write_chars(ANSI_LEFT); break;
					case KEY_RIGHT: write_chars(ANSI_RIGHT); break;
					case KEY_UP: break;
					case KEY_DOWN: break;
					case KEY_ESC: write_chars("\x07"); break;
				    }
				}
				// write("ESC:");
				continue;
			}
		    // else if 'delete' (127, 0x7F)
		    else if (c == 0x7F) {
				// if not at start of line
				if (pos>0) {
				    // write ANSI sequence "ESC [ 1 P" (delete 1 character)
				    write_chars("\b\x1B[1P");
				    pos -= 1;
				}
			}
		    // else if buffer not full
		    else if (pos < len-1) {
		        // write character
		        write_char(c);  // to screen
	                line[pos] = c;  // to line buffer
		        pos += 1;
		    }
		}
	
	/*
	char digits[32]; 
	i32_2_str_nt_0d((i32) c, digits, 32);
        write(digits);
	write(".");
	*/
    }
}










