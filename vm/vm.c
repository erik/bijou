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
#define OPCODES             for(;;) {switch(OPCODE) {
#define END_OPCODES         default: {printf("Don't know Opcode: %d\n", OPCODE); return;} }}
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

void bijou_interpret(void)
{
  // UNUSED(vm);
  bInst *ip = B_ALLOCN(bInst, 10);
  bInst i = *ip;
  size_t x;
  ip[1] = CREATE_ABC(OP_MOVE, 1, 2, 3);
  for (x = 0; x < 10; x++) {
    printf("%d: %d\n", x, ip[x]);
  }
  
  OPCODES;
     OP(NOP):       DISPATCH;
  END_OPCODES;
}

