/* Copyright 2010 Erik Price */
/*
 * Opcodes for the Bijou virtual machine
 */

#ifndef _BOPCODES_H_
#define _BOPCODES_H_


/* Ripped from Lua */
#define cast(T,X) ((T)X)

#define SIZE_C    9
#define SIZE_B    9
#define SIZE_Bx   (SIZE_C + SIZE_B)
#define SIZE_A    8

#define SIZE_OP   6

#define POS_OP    0
#define POS_A     (POS_OP + SIZE_OP)
#define POS_C     (POS_A + SIZE_A)
#define POS_B     (POS_C + SIZE_C)
#define POS_Bx    POS_C

//#if SIZE_Bx < TR_BITSINT-1
#define MAXARG_Bx   ((1<<SIZE_Bx)-1)
#define MAXARG_sBx  (MAXARG_Bx>>1)         /* `sBx' is signed */
//#else
//#define MAXARG_Bx   MAX_INT
//#define MAXARG_sBx  MAX_INT
//#endif

/* creates a mask with `n' 1 bits at position `p' */
#define MASK1(n,p)  ((~((~(bInst)0)<<n))<<p)

/* creates a mask with `n' 0 bits at position `p' */
#define MASK0(n,p)  (~MASK1(n,p))

/* the following macros help to manipulate instructions (bInst) */


#define GET_OPCODE(i) (cast(int, ((i)>>POS_OP) & MASK1(SIZE_OP,0)))
#define SET_OPCODE(i,o) ((i) = (((i)&MASK0(SIZE_OP,POS_OP)) | \
    ((cast(bInst, o)<<POS_OP)&MASK1(SIZE_OP,POS_OP))))

#define GETARG_A(i) (cast(int, ((i)>>POS_A) & MASK1(SIZE_A,0)))
#define SETARG_A(i,u) ((i) = (((i)&MASK0(SIZE_A,POS_A)) | \
    ((cast(bInst, u)<<POS_A)&MASK1(SIZE_A,POS_A))))

#define GETARG_B(i) (cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))
#define SETARG_B(i,b) ((i) = (((i)&MASK0(SIZE_B,POS_B)) | \
    ((cast(bInst, b)<<POS_B)&MASK1(SIZE_B,POS_B))))

#define GETARG_C(i) (cast(int, ((i)>>POS_C) & MASK1(SIZE_C,0)))
#define SETARG_C(i,b) ((i) = (((i)&MASK0(SIZE_C,POS_C)) | \
    ((cast(bInst, b)<<POS_C)&MASK1(SIZE_C,POS_C))))

#define GETARG_Bx(i)  (cast(int, ((i)>>POS_Bx) & MASK1(SIZE_Bx,0)))
#define SETARG_Bx(i,b)  ((i) = (((i)&MASK0(SIZE_Bx,POS_Bx)) | \
    ((cast(bInst, b)<<POS_Bx)&MASK1(SIZE_Bx,POS_Bx))))

#define GETARG_sBx(i) cast(int, GETARG_Bx(i)-MAXARG_sBx)
#define SETARG_sBx(i,b) SETARG_Bx((i),cast(unsigned int, (b)+MAXARG_sBx))

#define CREATE_ABC(o,a,b,c) ((cast(bInst, o)<<POS_OP) \
      | (cast(bInst, a)<<POS_A) \
      | (cast(bInst, b)<<POS_B) \
      | (cast(bInst, c)<<POS_C))

#define CREATE_ABx(o,a,bc)  ((cast(bInst, o)<<POS_OP) \
      | (cast(bInst, a)<<POS_A) \
      | (cast(bInst, bc)<<POS_Bx))
#define CREATE_AsBx(o, a, sbx) {bInst __i = CREATE_ABx(o, a, 0), SETARG_sBx(__i, sbx), __i }

/* Macros on RK values */

/* this bit means constant value or register
 * a value of 1 is constant
 */
#define BITRK      (1 << (SIZE_B - 1))

#define MAKEK(x) (x | BITRK)

/* test whether a given value is a constant */
#define ISK(x)     ((x) & BITRK)

/* determine index of a constant */
#define INDEXRK(r) ((int)(r) & ~BITRK)

/* maximmum value for a constant index */
#define MAXINDEXRK (BITRK - 1)

/* create a constant index */
#define RKASK(x)   ((x) | BITRK)

/* If you change the order of these, grep 'OPCODE ORDER' */
/*
 * Opcodes
 * R[x]  - Register x
 * K[x]  - Constant at index x
 * RK[x] - Register x or constant index x
*/
typedef enum {
    /* name            operands description */
    OP_NOP,         /*          Absolutely nothing. */

    OP_MOVE,        /* A B      R[A] = R[B] */
    OP_LOADK,       /* A Bx     R[A] = K[Bx] */
    OP_LOADBOOL,    /* A B      R[A] = (bool)B; */
    OP_LOADNULL,     /* A        R[A] = null */
    OP_GETGLOBAL,   /* A Bx     R[A] = globals[K[Bx]] */
    OP_SETGLOBAL,   /* A Bx     globals[K[Bx]] = R[A] */
    OP_GETLOCAL,    /* A Bx     R[A] = locals[K[Bx]] */
    OP_SETLOCAL,    /* A Bx     locals[K[Bx]] = R[A]*/

    /* Math ops */
    OP_ADD,         /* A B C    R[A] = RK[B] + RK[C] */
    OP_SUB,         /* A B C    R[A] = RK[B] - RK[C] */
    OP_MUL,         /* A B C    R[A] = RK[B] * RK[C] */
    OP_DIV,         /* A B C    R[A] = RK[B] / RK[C] */
    OP_POW,         /* A B C    R[A] = RK[B] ** RK[C] */
    OP_REM,         /* A B C    R[A] = RK[B] % RK[C] */
    OP_UNM,         /* A B      R[A] = -RK[B] */

    OP_NOT,         /* A B      R[A] = !RK[B] */

    OP_EQ,          /* A B      if (RK[A] == RK[B]) pc++ */
    OP_LT,          /* A B      if (RK[A] <  RK[B]) pc++ */
    OP_GT,          /* A B      if (RK[A] >  RK[B]) pc++ */
    OP_LE,          /* A B      if (RK[A] <= RK[B]) pc++ */
    OP_GE,          /* A B      if (RK[A] >= RK[B]) pc++ */

    OP_JMP,         /* sBx      pc += sBx */
    OP_CALL,        /* A B C    R[A] = R[B](R[B + 1], ..., R[B + 1 + C]) (see explanation below) */
    OP_CLOSURE,     /* A Bx     R[A] = FuncProtos[Bx] (see explanation below) */
    OP_RETURN,      /* A        return RK[A] */
    NUM_OPS         /* Number of opcodes */
} OpCodes;

/* for OP_CALL, a function pointer is expected to be contained in R[B].
 * C is the number of arguments to pass to the function. R[B] is then
 * called with values R[B + 1], ..., R[B + C]
 * the result is stored in R[A]
*/

/* for OP_CLOSURE, the value loaded into R[A] is the function described
 * in the local function prototype table at index Bx for use with OP_CALL
*/

/* for OPCODE_ARGS bitmask magic */
#define ARG_NONE (1 << 0)
#define ARG_A    (1 << 1)
#define ARG_B    (1 << 2)
#define ARG_C    (1 << 3)
#define ARG_Bx   (1 << 4)
#define ARG_sBx  (1 << 5)

#define HASARG_A(i)   ((i) & ARG_A)
#define HASARG_B(i)   ((i) & ARG_B)
#define HASARG_C(i)   ((i) & ARG_C)
#define HASARG_Bx(i)  ((i) & ARG_Bx)
#define HASARG_sBx(i) ((i) & ARG_sBx)

/* specify the number of arguments each
 * opcode takes. Used for the print_op
 * helper function
 */
#define OPCODE_ARGS \
  ARG_NONE,				\
  ARG_A | ARG_B,			\
  ARG_A | ARG_Bx,			\
  ARG_A | ARG_B,			\
  ARG_A,				\
  /* getglobal */  ARG_A | ARG_Bx,      \
  ARG_A | ARG_Bx,			\
  ARG_A | ARG_Bx,			\
  ARG_A | ARG_Bx,			\
  /* add */ ARG_A | ARG_B | ARG_C,	\
  ARG_A | ARG_B | ARG_C,		\
  ARG_A | ARG_B | ARG_C,		\
  ARG_A | ARG_B | ARG_C,		\
  ARG_A | ARG_B | ARG_C,		\
  ARG_A | ARG_B | ARG_C,		\
  /* unm */ ARG_A | ARG_B, 		\
  ARG_A | ARG_B,			\
  ARG_A | ARG_B,			\
  ARG_A | ARG_B,			\
  ARG_A | ARG_B,			\
  ARG_A | ARG_B,			\
  /* ge */ ARG_A | ARG_B,		\
  ARG_sBx,				\
  ARG_A | ARG_B | ARG_C,		\
  ARG_A | ARG_Bx,			\
  ARG_A

#define OPCODE_LABELS "nop",			\
    "move",					\
    "loadk",					\
    "loadbool",					\
    "loadnull",					\
    "getglobal",				\
    "setglobal",				\
    "getlocal",					\
    "setlocal",					\
    "add",					\
    "sub",					\
    "mul",					\
    "div",					\
    "pow",					\
    "rem",					\
    "unm",					\
    "not",					\
    "eq",					\
    "lt",					\
    "gt",					\
    "le",					\
    "ge",					\
    "jmp",					\
    "call",					\
    "closure",					\
    "return"

#endif /* _BOPCODES_H_ */
