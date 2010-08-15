#include "bijou.h"
#include "internal.h"
#include "bopcodes.h"
#include "vm.h"
#include "dump.h"
#include "load.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int usage(void)
{
    fprintf(stderr, "Usage:\nbijou [options] file\n"
            "-d\tDump bytecode in human readable format\n"
            "-v\tDisplay version and exit\n"
            "-h\tDisplay this help text\n");
    return 1;
}


static int reader(LoadState* S, u_byte* b, size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        int c = fgetc(S->fp);
        b[i] = c;
    }
    return i;
}

int main(int argc, char ** argv)
{
    char* inputfile = "";
    int dump = 0;

    int i;

    if (argc <= 1) {
        return usage();
    }

    for (i = 1; i < argc; ++i) {
        char *arg = argv[i];

        if (arg[0] == '-') {
            switch (arg[1]) {
            case 'd':
                dump = 1;
                break;
            case 'v': {
                int maj = (BIJOU_VERSION & 0xF0) >> 4;
                int min = BIJOU_VERSION & 0x0F;

                printf("Bijou v%X.%X\n", maj, min);
                exit(0);
            }
            case 'h':
                return usage();

            default:
                fprintf(stderr, "Unrecognized switch -%c\n", arg[1]);
                return usage();
            }
        } else {
            if (strlen(inputfile))
                return usage();

            inputfile = argv[i];
        }
    }

    if (strlen(inputfile) == 0) {
        return usage();
    }

    FILE* in = fopen(inputfile, "rb");

    if (in == NULL) {
        fprintf(stderr, "%s: no such file\n", inputfile);
        exit(1);
    }

    BijouVM* vm = BijouVM_new(0);
    BijouBlock* b = BijouBlock_new(0);
    BijouFrame* frame = B_ALLOC(BijouFrame);

    Proto* p = bijou_load(vm, b, (bijou_Reader)reader, inputfile, in);

    BijouBlock_destroy(b);

    b = proto_to_block(vm, p);

    if (dump) {
        BijouBlock_dump(b);
    }

    TValue val = bijou_interpret(vm, frame, b, 0, 0, NULL);
    char * str = TValue_to_string(val);

    printf("Interpret returned: %s\n", str);

    if (ttisnumber(&val)) B_FREE(str);

    BijouVM_destroy(vm);
    BijouBlock_destroy(b);
    Proto_destroy(p);
    fclose(in);
    B_FREE(frame->stack);
    B_FREE(frame);
    return 0;

}


