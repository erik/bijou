#include <stdio.h>

#include "bijou.h"
#include "internal.h"
#include "bopcodes.h"
#include "vm.h"

BijouVM *BijouVM_new(size_t numglobals)
{
    BijouVM *v = B_ALLOC(BijouVM);
    v->globals = B_MALLOC(sizeof(TValue) * numglobals);
    v->numglobals = numglobals;
    kv_init(v->functions);

    return v;
}

void BijouVM_destroy(VM)
{
    kv_free(vm->functions);
    B_FREE(vm->globals);
    B_FREE(vm);
    vm = NULL;
}

/*int BijouVM_push_global(VM, TValue val)
{
    size_t i;
    for (i = 0; i < vm->numglobals; ++i) {
      if (TValue_equal(vm->globals[i], val))
            return -1;
        kv_push(TValue, vm->globals, val);
    }
    return vm->nums - 1;
    }*/

int BijouVM_find_global(VM, TValue v)
{
    size_t i;
    for (i = 0; i < vm->numglobals; ++i)
        if (TValue_equal(vm->globals[i], v))
            return i;
    return -1;
}

/* dispatch macros */
#define NEXT_INST           { (i = *++pc); }
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
#define G(x)                (globals[x])
#define R                   stack
#define RK(x)               (ISK(x) ? K[x & ~0x100] : R[x])
#define RETURN(x)           return (x)
#define dojump(pc, i)       (pc) += (i)
#define EXIT                return create_none()


TValue bijou_interpret(VM, BijouFrame *f, BijouBlock *b, int start, int argc, TValue argv[] /*Closure *closure*/)
{
    UNUSED(vm);
    UNUSED(argc);
    UNUSED(argv);

//    BijouBlock_dump(b);

    assert(b->code.a && "Null pointer madness!");

    f->stack             = B_MALLOC(sizeof(TValue) * b->regc);
    bInst *pc            = b->code.a + start;
    bInst i              = *pc;
    TValue *locals       = b->locals.a;
    TValue *K            = b->k.a;
    TValue *stack        = f->stack;
    TValue *globals      = vm->globals;

    size_t x = 0;

    for (x = 0; pc <= b->code.a + start + kv_size(b->code); x++) {
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

            if (Bx >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access "
                        "global index %zu (%zu exist)\n", x, "setlocal", Bx, vm->numglobals);
                EXIT;
            }

            TValue tvindex = globals[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " global index %zu (type %s) instead of expected %s\n",
                        x, "setlocal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " global index %zu (%zu exist)\n",
                        x, "setlocal", index, vm->numglobals);
                EXIT;
            }

            R[A] = globals[index];
            DISPATCH;
        }

        case OP_SETGLOBAL: {

            if (Bx >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " global index %zu (%zu exist)\n",
                        x, "setlocal",
                        Bx, vm->numglobals);
                EXIT;
            }

            TValue tvindex = globals[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " global index %zu (type %s) instead of expected %s\n",
                        x, "setlocal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to acces"
                        "s global index %zu (%zu exist)\n",
                        x, "setlocal", index, vm->numglobals);
                EXIT;
            }

            globals[index] = R[A];
            DISPATCH;
        }

        case OP_GETLOCAL: {

            if (Bx >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " constant index %zu (%zu exist)\n",
                        x, "setlocal", Bx, kv_size(b->locals));
                EXIT;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " constant index %zu (type %s) instead of expected %s\n",
                        x, "setlocal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " local index %zu (%zu exist)\n",
                        x, "setlocal", index, kv_size(b->locals));
                EXIT;

            }

            R[A] = locals[index];
            DISPATCH;
        }

        case OP_SETLOCAL: {

            if (Bx >= kv_size(b->k)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " constant index %zu (%zu exist)\n",
                        x, "setlocal", Bx, kv_size(b->k));
                EXIT;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " constant index %zu (type %s) instead of expected %s\n",
                        x, "setlocal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " local index %zu (%zu exist)\n",
                        x, "setlocal", index, kv_size(b->locals));
                EXIT;
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
            DISPATCH;
        }
        case OP_SUB: {
            TValue t = TValue_num_sub(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            DISPATCH;
        }
        case OP_MUL: {
            TValue t = TValue_num_mul(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            DISPATCH;
        }
        case OP_DIV: {
            TValue t = TValue_num_div(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            DISPATCH;
        }

        case OP_POW: {
            TValue t = TValue_num_pow(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            DISPATCH;
        }
        case OP_REM: {
            TValue t = TValue_num_rem(RK(B), RK(C));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            DISPATCH;
        }
        case OP_UNM: {
            TValue t = TValue_num_unm(RK(B));
            if (t.tt == BIJOU_TNONE) {
                DISPATCH;
            }
            R[A] = t;
            DISPATCH;
        }

        case OP_NOT: {
            R[A] = create_boolean(! bvalue(RK(B)));
            DISPATCH;
        }

        case OP_EQ: {
            if (TValue_equal(RK(A), RK(B))) {
                ++pc;
            }
            DISPATCH;
        }

        case OP_LT: {
            if (bvalue(TValue_num_lt(RK(A), RK(B)))) {
                ++pc;
            }
            DISPATCH;
        }

        case OP_GT: {
            if (bvalue(TValue_num_gt(RK(A), RK(B)))) {
                ++pc;
            }
            DISPATCH;
        }

        case OP_LE: {
            if (bvalue(TValue_num_le(RK(A), RK(B)))) {
                ++pc;
            }
            DISPATCH;
        }

        case OP_GE: {
            if (bvalue(TValue_num_ge(RK(A), RK(B)))) {
                ++pc;
            }
            DISPATCH;
        }

        case OP_JMP: {
            dojump(pc, sBx);
            DISPATCH;
        }

        case OP_RETURN: {
            return RK(A);
        }

        case OP_CALL: {
            printf("TODO: OP_CALL\n");
            DISPATCH;
        }

        case OP_CLOSURE: {
            printf("TODO: OP_CLOSURE\n");
            DISPATCH;
        }

        default: {
            printf("Don't know Opcode: %zu\n", OPCODE);
            return create_none();
        }
        }
    }
    return create_none();
}

