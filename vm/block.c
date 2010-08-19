/* Copyright 2010 Erik Price */
/* Various functions to create and manipulate
 * structs of type BijouBlock
*/

#include "bopcodes.h"
#include "internal.h"
#include "bijou.h"
#include "vendor/kvec.h"
#include "dump.h"
#include "vm.h"

static const char * opcode_names[] = { OPCODE_LABELS };
static const int  opcode_args[] = { OPCODE_ARGS };

static void print_op(bInst);

BijouBlock *BijouBlock_new(BijouBlock *parent)
{
    BijouBlock *b = B_ALLOC(BijouBlock);
    kv_init(b->k);
    kv_init(b->code);
    kv_init(b->locals);
    kv_init(b->upvals);
    b->regc = 0;
    b->filename =  "";
    b->parent = parent;
    b->numchildren = 0;
    b->children = NULL;
    return b;
}

void BijouBlock_destroy(BijouBlock *b)
{
    kv_free(b->k);
    kv_free(b->locals);
    kv_free(b->upvals);
    kv_free(b->code);

    size_t i;
    for (i = 0; i < b->numchildren; ++i) {
        BijouBlock_destroy(b->children[i]);
    }

    B_FREE(b);
    b = NULL;
}


/* push a TValue into constants of block
 * returns index, or -1 if the value has
 * already been added
 */
int BijouBlock_push_const(BijouBlock *b, TValue v)
{
    size_t i;
    for (i = 0; i < kv_size(b->k); ++i) {
        if (TValue_equal(kv_A(b->k, i), v))
            return -1;
    }
    kv_push(TValue, b->k, v);
    return kv_size(b->k) - 1;
}

/* push a TValue into local stack, returning
 * index of local
 */
int BijouBlock_push_local(BijouBlock *b, TValue v)
{
    size_t i;
    for (i = 0; i < kv_size(b->locals); ++i) {
        if (TValue_equal(kv_A(b->locals, i), v))
            return -1;
    }
    kv_push(TValue, b->locals, v);
    return kv_size(b->locals) - 1;
}

/* search for constant matching TValue v
 * return index of match, or -1 if not found
 */
int BijouBlock_find_const(BijouBlock *b, TValue v)
{
    size_t i;
    for (i = 0; i < kv_size(b->k); i++) {
        if (TValue_equal(v, kv_A(b->k, i)))
            return i;
    }
    return -1;
}

/* search for local matching TValue v
 * return index of match, or -1 if not found
 */
int BijouBlock_find_local(BijouBlock *b, TValue v)
{
    size_t i;
    for (i = 0; i < kv_size(b->locals); i++) {
        if (TValue_equal(v, kv_A(b->locals, i)))
            return i;
    }
    return -1;
}

/* push a string into the constants
 * returns index
 */
int BijouBlock_push_string(BijouBlock *b, BijouString string)
{
    TValue t;
    Value v;

    v.s = string;

    t.tt = BIJOU_TSTRING;
    t.value = v;

    return BijouBlock_push_const(b, t);

}

/* search for string matching TValue v
 * return index of match, or -1 if not found
 */
int BijouBlock_find_string(BijouBlock *b, BijouString str)
{

    TValue t;
    Value v;

    v.s = str;

    t.tt = BIJOU_TSTRING;
    t.value = v;

    return BijouBlock_find_const(b, t);
}

/* pushes an instruction into the block. returns the index */
int BijouBlock_push_instruction(BijouBlock *b, bInst inst)
{
    kv_push(bInst, b->code, inst);
    return kv_size(b->code) - 1;
}

/* returns instruction at index, or -1 if index is greater
 * than number of instructions
 */
bInst BijouBlock_fetch_instruction(BijouBlock *b, int index)
{
    if (index >= (int)kv_size(b->code))
        return -1;
    return kv_A(b->code, index);
}

void BijouBlock_dump2(VM, BijouBlock*, int);

/* prints out data contained in BijouBlock b in
 * a human readable format (for debugging, etc.)
 */
void BijouBlock_dump(VM, BijouBlock *b)
{
    BijouBlock_dump2(vm, b, 0);
}

/*
 * Push a nested block (function)
 * returns index at which function is inserted
 */
int BijouBlock_push_child(BijouBlock* block, BijouBlock* child)
{
    int index = block->numchildren;

    block->numchildren++;
    block->children = B_REALLOC(block->children, block->numchildren);

    block->children[index] = child;
    return index;
}

#define INDENT { int __i; for(__i = 0; __i < level; ++__i) printf("\t"); }

void BijouBlock_dump2(VM, BijouBlock* b, int level)
{
    char * str;
    size_t x;
    INDENT;
    printf("; block at: %p, (level %d)\n", (void *)b, level);
    INDENT;
    printf("; %zu registers\n", b->regc);

    INDENT;
    printf("; constants (%zu)\n", kv_size(b->k));
    for (x = 0; x < kv_size(b->k); ++x) {
        str = TValue_to_string(kv_A(b->k, x));
        int s = ttisstring(&kv_A(b->k, x));
        INDENT;
        printf("\t%zu: (%s) %s%s%s\n", x, TValue_type_to_string(kv_A(b->k, x)), s ? "\"" : "",
               str, s ? "\"" : "");

        if (ttisnumber(&kv_A(b->k, x))) B_FREE(str);
    }

    INDENT;
    printf("; locals (%zu)\n", kv_size(b->locals));
    for (x = 0; x < kv_size(b->locals); ++x) {
        str = TValue_to_string(kv_A(b->locals, x));
        INDENT;
        printf("\t%zu: (%s) %s\n", x, TValue_type_to_string(kv_A(b->locals, x)), str);
        if (ttisnumber(&kv_A(b->locals, x))) B_FREE(str);
    }

    INDENT;
    printf("; upvals (%zu)\n", kv_size(b->upvals));
    for (x = 0; x < kv_size(b->upvals); ++x) {
        str = TValue_to_string(kv_A(b->upvals, x));
        INDENT;
        printf("\t%zu: (%s) %s\n", x, TValue_type_to_string(kv_A(b->upvals, x)), str);
        if (ttisnumber(&kv_A(b->upvals, x))) B_FREE(str);
    }

    INDENT;
    printf("; code section (%zu instructions)\n", kv_size(b->code));
    for (x = 0; x < kv_size(b->code); ++x) {
        bInst i = kv_A(b->code, x);
        INDENT;
        print_op(i);
        printf("\t");

        switch (GET_OPCODE(i)) {
        case OP_MOVE:
            printf("; R[%d] = R[%d]", GETARG_A(i), GETARG_B(i));
            break;
        case OP_LOADK:
            printf("; R[%d] = K[%d]", GETARG_A(i), GETARG_Bx(i));
            break;
        case OP_LOADBOOL:
            printf("; R[%d] = %s", GETARG_A(i), GETARG_B(i) == 0 ? "false" : "true" );
            break;
        case OP_LOADNULL:
            printf("; R[%d] = null", GETARG_A(i));
            break;
        case OP_GETGLOBAL:
            printf("; R[%d] = globals[K[%d]]", GETARG_A(i), GETARG_Bx(i));
            break;
        case OP_SETGLOBAL:
            printf("; globals[K[%d]] = R[%d]", GETARG_Bx(i), GETARG_A(i));
            break;
        case OP_GETLOCAL:
            printf("; R[%d] = locals[K[%d]]", GETARG_A(i), GETARG_Bx(i));
            break;
        case OP_SETLOCAL:
            printf("; locals[K[%d]] = R[%d]", GETARG_Bx(i), GETARG_A(i));
            break;
        case OP_ADD:
            printf("; R[%d] = RK[%d] + RK[%d]", GETARG_A(i), GETARG_B(i),
                   GETARG_C(i));
            break;
        case OP_SUB:
            printf("; R[%d] = RK[%d] - RK[%d]", GETARG_A(i), GETARG_B(i),
                   GETARG_C(i));
            break;
        case OP_MUL:
            printf("; R[%d] = RK[%d] * RK[%d]", GETARG_A(i), GETARG_B(i),
                   GETARG_C(i));
            break;
        case OP_DIV:
            printf("; R[%d] = RK[%d] / RK[%d]", GETARG_A(i), GETARG_B(i),
                   GETARG_C(i));
            break;
        case OP_POW:
            printf("; R[%d] = RK[%d] ** RK[%d]", GETARG_A(i), GETARG_B(i),
                   GETARG_C(i));
            break;
        case OP_REM:
            printf("; R[%d] = RK[%d] %% RK[%d]", GETARG_A(i), GETARG_B(i),
                   GETARG_C(i));
            break;
        case OP_UNM:
            printf("; R[%d] = -RK[%d]", GETARG_A(i), GETARG_B(i));
            break;
        case OP_NOT:
            printf("; R[%d] = !RK[%d]", GETARG_A(i), GETARG_B(i));
            break;
        case OP_CLOSURE:
            printf("; R[%d] = ", GETARG_A(i));
            if (ISK(GETARG_Bx(i))) {
                BijouFunction *func = kv_A(vm->functions, GETARG_Bx(i) & ~0x100);
                printf("%s\n", func->name);
            } else
                printf("closure[%d]\n", GETARG_Bx(i));
            break;
        case OP_CALL:
            printf("; R[%d] = R[%d](", GETARG_A(i), GETARG_B(i));
            size_t x;
            for (x = 0; x < GETARG_C(i); ++x) {
                printf("R[%zu], ", x);
            }
            if (x > 0) {
                printf("\b\b");
            }
            printf(")");
            break;
        }
        printf("\n");
    }
    INDENT;
    printf("; functions (%zu definitions)\n", b->numchildren);
    for (x = 0; x < b->numchildren; ++x) {
        BijouBlock_dump2(vm, b->children[x], level + 1);
    }
}

/* helper function to print out the opcode
 * as well as the arguments
 */
static void print_op(bInst op)
{
    int args = opcode_args[GET_OPCODE(op)];
    printf("\t%s", opcode_names[GET_OPCODE(op)]);
    if (args == ARG_NONE)
        return;
    if (HASARG_A(args))
        printf(" %d", GETARG_A(op));
    if (HASARG_B(args))
        printf(" %d", GETARG_B(op));
    if (HASARG_C(args))
        printf(" %d", GETARG_C(op));
    if (HASARG_Bx(args))
        printf(" %d", GETARG_Bx(op));
    if (HASARG_sBx(args))
        printf(" %d", GETARG_sBx(op));
    return;
}
