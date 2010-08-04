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
#define RK(x)               (ISK(x) ? K[x & ~0x100] : R[x])
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
    TValue *locals       = b->locals.a;
    TValue *K            = b->k.a;
    BijouString *strings = b->strings.a;
    TValue *stack        = f->stack;

    UNUSED(strings);
    size_t x = 0;

    /* TODO: this doesn't allow for jumps to happen... */
    for (x = 0; x < kv_size(b->code); x++) {
        /* TODO: GETGLOBAL */
        /* TODO: SETGLOBAL */
        switch (OPCODE) {
        case OP_NOP:
            DISPATCH;
        case OP_MOVE: {
            R[A] = R[B];
            DISPATCH;
        }
        case OP_LOADK: {
            R[A] = K[Bx];
            DISPATCH;
        }
        case OP_LOADBOOL: {
            R[A] = create_boolean(B);
            DISPATCH;
        }
        case OP_LOADNULL: {
            R[A] = create_null();
            DISPATCH;
        }
        case OP_GETGLOBAL: {
            printf("TODO: GETGLOBAL\n");
            DISPATCH;
        }
        case OP_SETGLOBAL: {
            printf("TODO: GETGLOBAL\n");
            DISPATCH;
        }
        case OP_GETLOCAL: {

            if (Bx >= kv_size(b->k)) {
                fprintf(stderr, "ERROR: [instruction %d (%s) tried to access constant index %d (%d exist)\n",
                        x, "setlocal", Bx, kv_size(b->k));
                DISPATCH;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %d (%s) tried to use constant index %d (type %s) instead of expected %s\n",
                        x, "setlocal", Bx, TValue_type_to_string(tvindex), "number");
                DISPATCH;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %d (%s)] tried to access local index %d (%d exist)\n",
                        x, "setlocal", index, kv_size(b->locals));
                DISPATCH;
                //return;
            }

            R[A] = locals[index];
            DISPATCH;
        }

        case OP_SETLOCAL: {

            if (Bx >= kv_size(b->k)) {
                fprintf(stderr, "ERROR: [instruction %d (%s) tried to access constant index %d (%d exist)\n",
                        x, "setlocal",
                        Bx, kv_size(b->k));
                DISPATCH;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %d (%s) tried to use constant index %d (type %s) instead of expected %s\n",
                        x, "setlocal", Bx, TValue_type_to_string(tvindex), "number");
                DISPATCH;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %d (%s)] tried to access local index %d (%d exist)\n",
                        x, "setlocal", index, kv_size(b->locals));
                DISPATCH;
                //return;
            }

            locals[index] = R[A];
            DISPATCH;
        }

        /* condense this math section */
        case OP_ADD: {
            TValue t = TValue_num_add(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }
        case OP_SUB: {
            TValue t = TValue_num_sub(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }
        case OP_MUL: {
            TValue t = TValue_num_mul(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }
        case OP_DIV: {
            TValue t = TValue_num_div(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }

        case OP_POW: {
            TValue t = TValue_num_pow(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }
        case OP_REM: {
            TValue t = TValue_num_rem(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }
        case OP_UNM: {
            TValue t = TValue_num_unm(RK(B));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }

        case OP_NOT: {
            TValue x = RK(B);
            R[A] = create_boolean(!x.value.b);
            printf("R[%d] = %s\n", A, TValue_to_string(R[A]));
            DISPATCH;
        }

        default: {
            printf("Don't know Opcode: %d\n", OPCODE);
            return;
        }
        }
    }
}

