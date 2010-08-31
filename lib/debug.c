/* Copyright 2010 Erik Price */

/* Debugging functions */

#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "const.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SHOULD_BE(tval, type) if((tval).tt != (type)) { \
	fprintf(stderr, "Expected type %d,"	        \
	"but got %d (%s)\n", type, (tval).tt, TValue_to_string(tval)); \
	exit(1); \
    }

static time_t start;

/*
 * Args -
 *	value [TValue]
 * Returns -
 *	NULL
 */
int    args_inspect = 1;
TValue func_inspect(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    TValue value = argv[0];

    printf("%s (%s)\n", TValue_to_string(value), TValue_type_to_string(value));

    return create_null();
}

/*
 * Dumps values of globals
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_global_dump = 0;
TValue func_global_dump(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    printf("; globals\n");

    size_t i;
    for (i = 0; i < vm->numglobals; ++i) {
        TValue value = vm->globals[i];
        printf("\t[%d] %s (%s)\n", i, TValue_to_string(value), TValue_type_to_string(value));
    }

    return create_null();
}

/*
 * Dumps values of constants
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_const_dump = 0;
TValue func_const_dump(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    BijouVM_dump_constants(vm);

    printf("; constants\n");

    size_t i;
    for (i = 0; i < kv_size(vm->constants); ++i) {
        TValue value = kv_A(vm->constants, i);
        printf("\t[%d] %s (%s)\n", i, TValue_to_string(value), TValue_type_to_string(value));
    }

    return create_null();
}

/*
 * Dumps globally available functions
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_func_dump = 0;
TValue func_func_dump(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    BijouVM_dump_functions(vm);

    return create_null();
}

/*
 * Dumps current block (and all sub-blocks)
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_block_dump = 0;
TValue func_block_dump(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(argc);
    UNUSED(argv);

    BijouBlock_dump(vm, blk);

    return create_null();

}

/*
 * Dumps values of locals
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_local_dump = 0;
TValue func_local_dump(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(argc);
    UNUSED(argv);

    printf("; locals\n");

    size_t i;
    for (i = 0; i < kv_size(blk->locals); ++i) {
        TValue value = kv_A(blk->locals, i);
        printf("\t[%d] %s (%s)\n", i, TValue_to_string(value), TValue_type_to_string(value));
    }

    return create_null();
}

/*
 * Dumps values of upvals
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_upval_dump = 0;
TValue func_upval_dump(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(argc);
    UNUSED(argv);

    printf("; upvals\n");

    size_t i;
    for (i = 0; i < kv_size(blk->upvals); ++i) {
        TValue value = kv_A(blk->upvals, i);
        printf("\t[%d] %s (%s)\n", i, TValue_to_string(value), TValue_type_to_string(value));
    }

    return create_null();
}

/*
 * Begins timer
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_timer_start = 0;
TValue func_timer_start(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    printf("Timer started.\n");
    time(&start);

    return create_null();
}

/*
 * Ends timer
 * Args -
 *	NONE
 * Returns -
 *	NULL
 */
int    args_timer_end = 0;
TValue func_timer_end(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    time_t end;
    double diff;

    time(&end);
    diff = difftime(start, end);
    printf("Timer finished. Took %lf seconds\n", diff);


    return create_null();
}
