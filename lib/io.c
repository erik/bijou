/* Copyright 2010, Erik Price */

/* IO library. Equivalent to stdio.h in C */

#include "bijou.h"
#include "internal.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>

#define SHOULD_BE(tval, type) if((tval).tt != (type)) { \
	fprintf(stderr, "Expected type %d,"	        \
	"but got %d (%s)\n", type, (tval).tt, TValue_to_string(tval)); \
	exit(1); \
    }

/*
 * Args -
 *	filename [String]
 *	mode     [String]
 * Returns -
 *	file pointer [FILE *]
 */
int    args_open_file = 2;
TValue func_open_file(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    char *filename;
    char *mode;
    FILE *file;

    SHOULD_BE(argv[0], BIJOU_TSTRING);
    SHOULD_BE(argv[1], BIJOU_TSTRING);

    filename = argv[0].value.s.ptr;
    mode     = argv[1].value.s.ptr;

    file     = fopen(filename, mode);

    /* an error occured */
    if (file == NULL) {
        return create_null();
    }

    return create_pointer((void *)file);

}

/*
 * Args -
 *	file [FILE*]
 * Returns -
 *	null
 */
int    args_file_close = 1;
TValue func_file_close(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    FILE *file;

    SHOULD_BE(argv[0], BIJOU_TPOINTER);

    file = (FILE*)argv[0].value.pointer;

    fclose(file);

    return create_null();
}
