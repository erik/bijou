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
        BijouBlock_push_const(b, create_bijou_Number(42));
        BijouBlock_push_const(b, create_bijou_Number(-100));
        BijouBlock_push_const(b, create_boolean(0));


        BijouBlock_push_string(b, BijouString_new("a string"));
        BijouBlock_push_string(b, BijouString_new("another string"));

        /* fill up block with each opcode and
         * random args
         */
        size_t x;
        for (x = 0; x <= OP_RETURN; ++x) {
                bInst i = 0;
                SET_OPCODE(i, x);
                SETARG_A(i, 2);
                SETARG_B(i, 1);
                SETARG_C(i, 0);
                BijouBlock_push_instruction(b, i);
        }


        BijouFrame frame;

        bijou_interpret(&a, &frame, b, 0, 0, NULL );
        return usage();
}


