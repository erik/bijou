/* Copyright 2010 Erik Price */

#include "bijou.h"
#include "internal.h"
#include "bopcodes.h"
#include "vm.h"
#include "lib.h"
#include "func.h"
#include "const.h"

#include <string.h>
#include <stdio.h>

BijouVM *BijouVM_new(size_t numglobals)
{
    BijouVM *v = B_ALLOC(BijouVM);
    v->globals = B_MALLOC(sizeof(TValue) * numglobals);
    v->numglobals = numglobals;
    kv_init(v->functions);
    kv_init(v->constants);
    kv_init(v->libs);

    setup_internal_functions(v);
    setup_internal_constants(v);

    return v;
}

/* dumps the VM's internal functions */
void BijouVM_dump_functions(VM)
{

    printf("; Internal functions:\n");

    size_t i;
    for (i = 0; i < kv_size(vm->functions); ++i) {
        BijouFunction *f = kv_A(vm->functions, i);
        printf("\t%s (arity: %d)", f->name, f->arity);
        if (++i < kv_size(vm->functions)) {
            f = kv_A(vm->functions, i);
            printf("   \t%s (arity: %d)", f->name, f->arity);
        }
        printf("\n");

    }
}

/* dumps the VM's internal constants */
void BijouVM_dump_constants(VM)
{
    printf("; Internal constants:\n");

    size_t i;
    for (i = 0; i < kv_size(vm->constants); ++i) {
        TValue t = kv_A(vm->constants, i);
        printf("\t[%d] %s (%s)", i, TValue_to_string(t), TValue_type_to_string(t));
        if (++i < kv_size(vm->constants)) {
            t = kv_A(vm->constants, i);
            printf("   \t[%d] %s (%s)", i,  TValue_to_string(t), TValue_type_to_string(t));
        }
        printf("\n");
    }
}

void BijouVM_destroy(VM)
{
    size_t i;
    for (i = 0; i < kv_size(vm->functions); ++i) {
        BijouFunction_destroy(kv_A(vm->functions, i));
    }
    for (i = 0; i < kv_size(vm->libs); ++i) {
        LIB_CLOSE(kv_A(vm->libs, i));
    }
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

int BijouVM_push_function(VM, BijouFunction* func)
{
    int size = kv_size(vm->functions);
    kv_push(BijouFunction*, vm->functions, func);
    return size;

}

int BijouVM_push_constant(VM, TValue t)
{
    int size = kv_size(vm->constants);
    kv_push(TValue, vm->constants, t);
    return size;

}

int BijouVM_push_lib(VM, void* handle)
{
    int size = kv_size(vm->libs);
    kv_push(void *, vm->libs, handle);
    return size;
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


TValue bijou_interpret(VM, BijouFrame *f, BijouBlock *b, int start, int argc, TValue argv[])
{

    f->stack             = B_MALLOC(sizeof(TValue) * b->regc);
    bInst *pc            = b->code.a + start;
    bInst i              = *pc;
    TValue *locals       = b->locals.a;
    TValue *K            = b->k.a;
    TValue *stack        = f->stack;
    TValue *globals      = vm->globals;

    size_t x = 0;

    for (x = 0; pc <= b->code.a + start + kv_size(b->code); ++x) {

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
                        "global index %zu (%zu exist)\n", x, "getglobal", Bx, vm->numglobals);
                EXIT;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " global index %zu (type %s) instead of expected %s\n",
                        x, "getglobal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " global index %zu (%zu exist)\n",
                        x, "getglobal", index, vm->numglobals);
                EXIT;
            }

            R[A] = globals[index];
            DISPATCH;
        }

        case OP_SETGLOBAL: {

            if (Bx >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " global index %zu (%zu exist)\n",
                        x, "setglobal",
                        Bx, vm->numglobals);
                EXIT;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " global index %zu (type %s) instead of expected %s\n",
                        x, "setglobal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= vm->numglobals) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to acces"
                        "s global index %zu (%zu exist)\n",
                        x, "setglobal", index, vm->numglobals);
                EXIT;
            }

            globals[index] = R[A];
            DISPATCH;
        }

        case OP_GETLOCAL: {

            if (Bx >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " constant index %zu (%zu exist)\n",
                        x, "getlocal", Bx, kv_size(b->locals));
                EXIT;
            }

            TValue tvindex = K[Bx];

            if (! ttisnumber(&tvindex)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to use"
                        " constant index %zu (type %s) instead of expected %s\n",
                        x, "getlocal", Bx, TValue_type_to_string(tvindex), "number");
                EXIT;
            }

            size_t index = (int)TV2BN(tvindex);
            if (index >= kv_size(b->locals)) {
                fprintf(stderr, "ERROR: [instruction %zu (%s)] tried to access"
                        " local index %zu (%zu exist)\n",
                        x, "getlocal", index, kv_size(b->locals));
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

        case OP_GETARG: {

            if ((int)Bx >= argc) {
                fprintf(stderr, "ERROR: [instruction %zu (getarg) tried to use"
                        " argument index %zu, (%zu exist)\n",
                        x, Bx, argc);
                EXIT;
            }

            R[A] = argv[Bx];
            DISPATCH;
        }

        case OP_GETEXTERNAL: {

            TValue tval = K[B];

            if (tval.tt != BIJOU_TSTRING) {
                fprintf(stderr, "Expected a string for getexternal, but got %s\n", TValue_type_to_string(tval));
                exit(1);
            }

            if (kv_size(vm->libs) == 0) {
                fprintf(stderr, "Tried to call external function (%s), but no libs are linked!\n",
                        tval.value.s.ptr);
                exit(1);
            }

            char* func = B_MALLOC(5 + strlen(tval.value.s.ptr));
            char *func_args = B_MALLOC( 5 + strlen(tval.value.s.ptr));


            TValue (*fptr)(BijouVM*, BijouBlock*, int, TValue*);
            int *num_args;

            void *handle;

            int found = 0;
            size_t ctr;

            char* error;
            for (ctr = 0; ctr < kv_size(vm->libs); ++ctr) {
                B_FREE(func);
                B_FREE(func_args);

                handle = kv_A(vm->libs, ctr);

                if (handle == NULL) {
                    fprintf(stderr, "%s:%d: Error occured on dynamic load: %s\n", __FILE__, __LINE__, LIB_ERROR);
                    exit(1);
                }

                /* FIXME: Obviously, I'm doing it wrong here. */

                func = B_MALLOC(5 + strlen(tval.value.s.ptr));
                memset(func, '\0', 5 + strlen(tval.value.s.ptr));
                strcpy(func, "func_");
                strcpy(func + 5, tval.value.s.ptr);
                func[strlen(func)] = '\0';

                func_args = B_MALLOC( 5 + strlen(tval.value.s.ptr));
                memset(func_args, '\0', 5 +  strlen(tval.value.s.ptr));
                strcpy(func_args, "args_");
                strcpy(func_args + 5, tval.value.s.ptr);
                func_args[strlen(func_args)] = '\0';

                num_args = LIB_READ(handle, func_args);

                fptr = (BijouFunc *)LIB_READ(handle, func);


                error = LIB_ERROR;

                if (fptr != NULL && num_args != NULL) {
                    found = 1;
                    break;
                }
            }

            if (!found || error != NULL) {
                fprintf(stderr, "Error on dynamic load: %s\n", error);
                exit(1);
            }

            BijouFunction* f = BijouFunction_new((BijouFunc*)fptr, *num_args, tval.value.s.ptr, 2, NULL);
            R[A] = create_function(f);
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
            if (! ttisfunction(&R[B])) {
                fprintf(stderr, "Tried to call something other than a closure\n");
                exit(1);
            }
            BijouFunction *func = R[B].value.func;

            R[A] = BijouFunction_call(vm, b, *func, (R + 1) + B, C);

            DISPATCH;
        }

        case OP_CLOSURE: {

            /* built in function */
            if (ISK(Bx)) {
                if ((Bx & ~0x100) > kv_size(vm->functions)) {
                    fprintf(stderr, "Tried to access internal function %d (%d exist)\n", Bx, kv_size(vm->functions));
                    exit(1);
                }
                R[A] = create_function(kv_A(vm->functions, Bx & ~0x100));
            } else {
                if (Bx > b->numchildren) {
                    fprintf(stderr, "Tried to access function %d (%d exist)\n", Bx, b->numchildren);
                    exit(1);
                }
                int index = (int)Bx;

                BijouBlock* block = b->children[index];
                BijouFunction* func = BijouFunction_new(NULL, block->argc, block->funcname, 0, block);
                R[A] = create_function(func);
            }
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

