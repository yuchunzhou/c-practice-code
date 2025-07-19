#include "log.h"

int main(int argc, char *argv[]) {
    int int_data = 0x01020304;
    char char_data = *(char *)&int_data;
    if (char_data == 0x04) {
        log_info("little endianness");
    } else {
        log_info("big endianness");
    }

    return 0;
}
