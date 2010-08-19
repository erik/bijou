/* Copyright 2010 Erik Price */

/* standard functions */

#include "lib.h"
#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "func.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define setup_func(f, name, arity)  {					\
	func = BijouFunction_new((BijouFunc*)f, arity, name);	\
	BijouVM_push_function(vm, func);			\
    }


void setup_internal_functions(VM)
{
    srand(time(NULL));

    UNUSED(vm);
    BijouFunction *func;

    setup_func(func_print, "print", -1);
    setup_func(func_println, "println", -1);

    setup_func(func_sin, "sin", 1);
    setup_func(func_cos, "cos", 1);
    setup_func(func_tan, "tan", 1);

    setup_func(func_exp, "exp", 1);
    setup_func(func_log, "log", 1);

    setup_func(func_ceil, "ceil", 1);
    setup_func(func_floor, "floor", 1);

    setup_func(func_rand, "rand", 0);
    setup_func(func_rand_int, "rand_int", 1);

}

TValue func_print(VM, BijouBlock *b, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(b);

    char *str;

    int i;
    for (i = 0; i < argc; ++i) {
        str = TValue_to_string(argv[i]);
        printf("%s", str);

        if (ttisnumber(&argv[i]) || ttisfunction(&argv[i])) B_FREE(str);
    }

    return create_null();
}


TValue func_println(VM, BijouBlock *b, int argc, TValue* argv)
{
    func_print(vm, b, argc, argv);
    printf("\n");
    return create_null();
}

TValue func_sin(VM, BijouBlock *blk, int argc, TValue* argv)
{

    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(sinl(a));
}

TValue func_cos(VM, BijouBlock *blk, int argc, TValue* argv)
{

    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(cosl(a));
}

TValue func_tan(VM, BijouBlock *blk, int argc, TValue* argv)
{

    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(tanl(a));
}

TValue func_exp(VM, BijouBlock *blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(exp(a));
}

TValue func_log(VM, BijouBlock *blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(log(a));
}

TValue func_ceil(VM, BijouBlock *blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(ceil(a));
}

TValue func_floor(VM, BijouBlock *blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    bijou_Number a = numvalue(argv[0]);

    return create_bijou_Number(floor(a));
}

TValue func_rand(VM, BijouBlock *blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    return create_bijou_Number((rand() / ((double)RAND_MAX + 1)));
}

TValue func_rand_int(VM, BijouBlock *blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    return create_bijou_Number((long)((rand() / ((double)RAND_MAX + 1)) * numvalue(argv[0])));
}
