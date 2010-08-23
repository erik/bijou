#include "bijou.h"
#include "internal.h"
#include "vm.h"

int args_test = 0;
TValue func_test(VM, BijouBlock* blk, int argc, TValue* argv )
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    printf("Ohai! I works!\n");
    return create_bijou_Number(12);
}

