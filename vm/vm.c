#include <stdio.h>

#include "bijou.h"
#include "internal.h"
#include "bopcodes.h"



const TValue *to_number(const TValue *obj, TValue *n)
{
        if (ttisnumber(obj))
                return obj;
        /* TODO: fix this at some point to be less ugly */
        if (ttisstring(obj)) {
                setnumvalue(n, strtod(obj->value.s.ptr, NULL));
                return n;
        } else {
                return NULL;
        }
}

/* dispatch macros */
#define NEXT_INST           { (i = *++ip); }
#define OP(n)               case OP_##n
#define DISPATCH            NEXT_INST; break


/* Some macros for bijou_interpret */
#define runtime_check(c)    { if (!(c)) break; }

#define OPCODE              GET_OPCODE(i)
#define A                   GETARG_A(i)
#define B                   GETARG_B(i)
#define C                   GETARG_C(i)
#define Bx                  GETARG_Bx(i)
#define sBx                 GETARG_sBx(i)
#define R                   stack
#define RK(x)               (ISK(x) ? k[x & ~0x100] : R[x])
#define RETURN(x)           return (x)
#define dojump(pc, i)       (pc) += (i)

void bijou_interpret(VM, BijouFrame *f, BijouBlock *b, int start, int argc, TValue argv[] /*Closure *closure*/)
{
        UNUSED(vm);
        UNUSED(argc);
        UNUSED(argv);

        BijouBlock_dump(b);

        assert(b->code.a && "Null pointer madness!");

        f->stack             = B_MALLOC(sizeof(StkId) * b->regc);
        bInst *ip            = b->code.a + start;
        bInst i              = *ip;
        TValue *k            = b->k.a;
        BijouString *strings = b->strings.a;
        TValue *stack        = f->stack;

        UNUSED(strings);
        size_t x = 0;

	/* TODO: this doesn't allow for jumps to happen... */
        for (x = 0; x < kv_size(b->code); x++) {
                printf("Opcode: %d\n", OPCODE);
                switch (OPCODE) {
                        OP(NOP):       DISPATCH;
                        OP(MOVE):      {
                                R[A] = R[B];
                                DISPATCH;
                        }
                        OP(LOADK):     {
                                R[A] = k[Bx];
                                DISPATCH;
                        }
                        OP(LOADBOOL):  {
                                R[A] = create_boolean(B);
                                DISPATCH;
                        }
                default:
                        printf("Don't know Opcode: %d\n", OPCODE);
                        return;
                }
        }
}

