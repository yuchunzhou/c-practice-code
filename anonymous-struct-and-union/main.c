#include "log.h"

struct Foo {
    struct {
        int field1;
    };
    union {
        int field2;
        short int field3;
    };
};

static struct {
    int field;
} Bar;

int main(int argc, char *argv[]) {
    struct Foo foo = {.field1 = 1, .field3 = 3};
    log_info("0x%lx", *((long int *)&foo));
    log_info("%d %d", foo.field1, foo.field3);

    Bar.field = -1;
    log_info("%d", Bar);

    return 0;
}
