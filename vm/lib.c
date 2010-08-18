/* Copyright 2010 Erik Price */

/* standard functions */

#include "lib.h"
#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "func.h"

#include <math.h>

#define setup_func(f, name, arity)  {					\
	func = BijouFunction_new((BijouFunc*)f, arity, name);	\
	BijouVM_push_function(vm, *func);			\
	BijouFunction_destroy(func);				\
    }


void setup_internal_functions(VM)
{

    UNUSED(vm);
    BijouFunction *func;

    setup_func(func_print, "print", -1);
    setup_func(func_println, "println", -1);

    setup_func(func_sin, "sin", 1);
    setup_func(func_cos, "cos", 1);
    setup_func(func_tan, "tan", 1);

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
