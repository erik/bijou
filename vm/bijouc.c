/* Bijou assembler */

#include "bijouc.h"
#include "internal.h"
#include "bijou.h"
#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int usage()
{
    fprintf(stderr, "Usage:\nbijouc [options] file\n"
            "-o\tSpecify an output file (default is 'b.out')\n"
            "-v\tOutput version and exit\n"
            "-p\tParse only, don't generate output"
            "\n");
    return 1;
}

int main(int argc, char **argv)
{
    char* outputfile = "b.out";
    char* inputfile = "";

    unsigned int options  = 0;

    int i;

    if (argc <= 1) {
        return usage();
    }

    for (i = 1; i < argc; ++i) {
        char *arg = argv[i];

        if (arg[0] == '-') {
            switch (arg[1]) {
            case 'o':
                outputfile = argv[++i];
                break;
            case 'v': {
                int maj = (BIJOUC_VERSION & 0xF0) >> 4;
                int min = BIJOUC_VERSION & 0x0F;

                printf("Version %X.%X\n", maj, min);
                exit(0);
            }
            case 'p':
                options |= OPT_PARSE;
                break;
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
    FILE* out = fopen(outputfile, "wb");
    if (in == NULL) {
        fprintf(stderr, "%s: no such file\n", inputfile);
        exit(1);
    }
    return compile_file(in, out, options);
}
