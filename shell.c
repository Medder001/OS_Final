#include "shell.h"
#include "kernel.h"
#include "io.h"
#include "strings.h"
#include "sbi.h"
#include "O5.h"

void user_shell() {
    char line[256];

    BStrBuf64 buf;
    write_bstr(BStrBuf64_init_chars(&buf, "O5 Shell\n"));

    while (1) {
        write_chars(" > ");
        readln_poll(line, 256);

        if (str_nt_eq("", line)) {
            // nop
        } else if (str_nt_eq("exit", line)) {
            return;
        } else if (str_nt_eq("help", line)) {
            write_chars(
                    "list of commands:\n"
                    "  help - print this help\n"
                    "  info - print system information\n"
                    "  exit - leave this shell\n"
                    "  shutdown - shutdown system\n"
            );
        } else if (str_nt_eq("shutdown", line)) {
            o5_fini();
        } else if (str_nt_eq("fini", line)) {
            o5_fini();
        } else if (str_nt_eq("info", line)) {
            o5_info();
        } else {
            write_chars("[error] Command '");
            write_chars(line);
            write_chars("' is unknown!\n");
        }
    }
}