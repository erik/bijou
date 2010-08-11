#include "bijou.h"
#include "internal.h"
#include "bopcodes.h"
#include "vm.h"
#include "compiler.h"

#include <stdlib.h>
#include <stdio.h>

int usage(void)
{
    printf("\nUsage: \n\tFIXME: write\n");
    return 1;
}

static int writer(VM, const void *p, size_t s, void *d)
{
    UNUSED(vm);
    return (fwrite(p, s, 1, (FILE*)d) != 1) && (s != 0);
}

int main(int argc, char ** argv)
{
    UNUSED(argc);
    UNUSED(argv);
    printf("SIG: %s\n", BIJOU_SIGNATURE);

    BijouVM *a = BijouVM_new(10);

    BijouBlock *b = BijouBlock_new(0);
    BijouBlock_push_const(b, create_bijou_Number(1));
    BijouBlock_push_const(b, create_bijou_Number(2));
    BijouBlock_push_const(b, create_boolean(0));


    BijouBlock_push_string(b, BijouString_new("a string"));
    BijouBlock_push_string(b, BijouString_new("a string"));
    BijouBlock_push_string(b, BijouString_new("another string"));

    BijouBlock_push_local(b, create_bijou_Number(12));
    BijouBlock_push_local(b, create_bijou_Number(15));
    BijouBlock_push_local(b, create_boolean(0));

    /* wild guess for number needed */
    b->regc = 20;

    b->filename = "omgbytecode";
    /* fill up block with each opcode and
     * random args
     */
    /*size_t x;
    for (x = 0; x <= OP_RETURN; ++x) {
        bInst i = 0;
        SET_OPCODE(i, x);
        if (x >= OP_ADD && x <= OP_UNM) {
            SETARG_A(i, 0);
            SETARG_B(i, MAKEK(0));
            SETARG_C(i, MAKEK(0));
        } else if (x >= OP_EQ && x <= OP_GE) {
            SETARG_A(i, 0);
            SETARG_B(i, MAKEK(0));
        } else if (x == OP_JMP) {
            SETARG_sBx(i, 0);
        } else {
            SETARG_A(i, 2);
            SETARG_Bx(i, 0);
        }
        BijouBlock_push_instruction(b, i);
    }
    */

    BijouBlock_push_instruction(b, CREATE_ABC(OP_ADD, 0, MAKEK(0), MAKEK(1)));
    BijouBlock_push_instruction(b, CREATE_ABC(OP_RETURN, 0, 0, 0));

    Proto *p = to_proto(a, b);
    BijouFrame *frame = B_ALLOC(BijouFrame);

    TValue val = bijou_interpret(a, frame, b, 0, 0, NULL );
    char * str = TValue_to_string(val);
    printf("Interpret returned: %s\n", str);

    if (ttisnumber(&val)) B_FREE(str);

    FILE * out = fopen("b.out", "w");

    bijou_dump(a, p, writer, out);
    fclose(out);
    BijouBlock_destroy(b);
    BijouVM_destroy(a);
    Proto_destroy(p);
    B_FREE(frame->stack);
    B_FREE(frame);
    return usage();
}


