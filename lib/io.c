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

/*
 * Args -
 *	file  [FILE*]
 *	count [Integer]
 * Returns -
 *	read  [String]
 */
int    args_file_read = 2;
TValue func_file_read(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    FILE *file;
    long  size;
    char  *buf;

    SHOULD_BE(argv[0], BIJOU_TPOINTER);
    SHOULD_BE(argv[1], BIJOU_TNUMBER);

    file = (FILE*)argv[0].value.pointer;
    size = (long) argv[1].value.n;
    buf = B_MALLOC(size * sizeof(char));

    fread(buf, 1, size, file);

    return create_TValue_string(BijouString_new(buf));
}

/*
 * Args -
 *	file    [FILE*]
 *	string  [String]
 * Returns -
 *	written [Integer]
 */
int    args_file_write = 2;
TValue func_file_write(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    FILE *file;
    char  *buf;
    size_t size;
    BijouString str;

    SHOULD_BE(argv[0], BIJOU_TPOINTER);
    SHOULD_BE(argv[1], BIJOU_TSTRING);

    file = (FILE*)argv[0].value.pointer;
    str  = argv[1].value.s;

    buf = str.ptr;
    size = str.len;

    int written = fwrite(buf, 1, size, file);

    return create_bijou_Number(written);
}

/*
 * Args -
 *	file    [FILE*]
 * Returns -
 *	file contents [String]
 */
int    args_file_slurp = 1;
TValue func_file_slurp(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    FILE *file;
    long  filesize;
    char  *buf;

    SHOULD_BE(argv[0], BIJOU_TPOINTER);

    file = (FILE*)argv[0].value.pointer;

    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    rewind(file);

    buf = B_MALLOC(filesize * sizeof(char));

    fread(buf, 1, filesize, file);

    return create_TValue_string(BijouString_new(buf));
}
