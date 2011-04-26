/* Copyright 2010, Erik Price */

/* Bijou's string manipulation library */

#include "bijou.h"
#include "internal.h"
#include "vm.h"

#include <string.h>

#define SHOULD_BE(tval, type) if((tval).tt != (type)) { \
	fprintf(stderr, "Expected type %d,"	        \
	"but got %d (%s)\n", type, (tval).tt, TValue_to_string(tval)); \
	exit(1); \
    }


/*
 * Args -
 *	base [String]
 *	append [String]
 * Returns -
 *	base + append [String]
 */
int    args_string_cat = 2;
TValue func_string_cat(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    BijouString base;
    BijouString append;

    SHOULD_BE(argv[0], BIJOU_TSTRING);
    SHOULD_BE(argv[1], BIJOU_TSTRING);

    base   = argv[0].value.s;
    append = argv[1].value.s;

    BijouString ret = BijouString_cat(base, append);

    return create_TValue_string(ret);
}

/*
 * Args -
 *	string [String]
 *	index  [Integer]
 * Returns -
 *	character at index [String]
 */
int    args_char_at = 2;
TValue func_char_at(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    size_t index;

    SHOULD_BE(argv[0], BIJOU_TSTRING);
    SHOULD_BE(argv[1], BIJOU_TNUMBER);

    index = (size_t)argv[1].value.n;
    if (index >= argv[0].value.s.len) {
        fprintf(stderr, "char_at: index %lu out of range for string %s (length: %lu)\n",
                index, argv[0].value.s.ptr, argv[0].value.s.len);
        exit(1);
    }

    return create_TValue_string(BijouString_substr(argv[0].value.s, index, 1));
}

/*
 * Args -
 *	string [String]
 *	search [String]
 * Returns -
 *	index [Integer]
 */
int    args_index_of = 2;
TValue func_index_of(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    int index = -1;
    size_t search_size;

    BijouString string;
    BijouString search;

    SHOULD_BE(argv[0], BIJOU_TSTRING);
    SHOULD_BE(argv[1], BIJOU_TSTRING);

    string = argv[0].value.s;
    search = argv[1].value.s;

    search_size = BijouString_len(search);

    int i;
    for (i = 0; i < BijouString_len(string); i += search_size) {
        BijouString s = BijouString_substr(string, i, search_size);
        if (BijouString_equal(s, search)) {
            index = i;
            break;
        }
    }

    return create_bijou_Number(index);
}

/*
 * Args -
 *	string [String]
 *	begin  [Integer]
 *	length [Integer]
 * Returns -
 *	string[begin]..string[begin + length]
 */
int    args_substr = 3;
TValue func_substr(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    BijouString string;
    int index;
    int length;

    SHOULD_BE(argv[0], BIJOU_TSTRING);
    SHOULD_BE(argv[1], BIJOU_TNUMBER);
    SHOULD_BE(argv[2], BIJOU_TNUMBER);

    string =  argv[0].value.s;
    index  =  argv[1].value.n;
    length =  argv[2].value.n;

    return create_TValue_string(BijouString_substr(string, index, length));
}

/*
 * Args -
 *	string [String]
 * Returns -
 *	length [Integer]
 */
int    args_length = 1;
TValue func_length(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    SHOULD_BE(argv[0], BIJOU_TSTRING);

    return create_bijou_Number(BijouString_len(argv[0].value.s));
}

