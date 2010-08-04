#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "config.h"
#include "bopcodes.h"

#include <stdlib.h>

int usage(void)
{
    printf("Usage: \n");
    return 1;
}


int main(int argc, char ** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    printf("SIG: %s\n", BIJOU_SIGNATURE);

    struct BijouVM a;
    a.temp = 0;

    BijouBlock *b = BijouBlock_new();
    BijouBlock_push_const(b, create_bijou_Number(7));
    BijouBlock_push_const(b, create_bijou_Number(10));
    BijouBlock_push_const(b, create_boolean(0));


    BijouBlock_push_string(b, BijouString_new("a string"));
    BijouBlock_push_string(b, BijouString_new("another string"));

    BijouBlock_push_local(b, create_bijou_Number(12));
    BijouBlock_push_local(b, create_bijou_Number(15));
    BijouBlock_push_local(b, create_boolean(0));

    /* wild guess for number needed */
    b->regc = 20;

    /* fill up block with each opcode and
     * random args
     */
    size_t x;
    for (x = 0; x <= OP_RETURN; ++x) {
        bInst i = 0;
        SET_OPCODE(i, x);
        SETARG_A(i, 2);
        if (x >= OP_ADD && x <= OP_UNM) {
            SETARG_B(i, MAKEK(1));
            SETARG_C(i, MAKEK(0));
        } else
            SETARG_Bx(i, 0);
        BijouBlock_push_instruction(b, i);
    }


    BijouFrame frame;

    bijou_interpret(&a, &frame, b, 0, 0, NULL );
    return usage();
}


