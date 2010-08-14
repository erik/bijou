#ifndef _DUMP_H_
#define _DUMP_H_

/* size of header file */
#define BIJOU_HEADERSIZE      6


#define PUSH_INST_A(BLK, O, A)            BijouBlock_push_instruction(BLK, CREATE_ABC(OP_##O, A, 0, 0))
#define PUSH_INST_AB(BLK, O, A, B)        BijouBlock_push_instruction(BLK, CREATE_ABC(OP_##O, A, B, 0))
#define PUSH_INST_ABC(BLK, O, A, B, C)    BijouBlock_push_instruction(BLK, CREATE_ABC(OP_##O, A, B, C))
#define PUSH_INST_ABx(BLK, O, A, Bx)      BijouBlock_push_instruction(BLK, CREATE_ABx(OP_##O, A, Bx))
#define PUSH_INST_AsBx(BLK, O, A, sBx)    BijouBlock_push_instruction(BLK, CREATE_AsBx(OP_##O, A, sBx))


typedef struct BijouCompiler {
    int line;
    char * filename;
    BijouBlock *block;
    struct BijouVM *vm;
} BijouCompiler;


/* struct representation of bytecode format (for writing bytecode) */
typedef struct Proto {
    BijouString    source;
    uint32_t       linedefined;
    uint32_t       lastlinedefined;
    u_byte         maxregisters;
    u_byte         numglobal;
    u_byte         numupval;
    u_byte         numparam;

    uint32_t       sizek;
    TValue         *k;

    uint32_t       sizecode;
    bInst          *code;

    uint32_t       sizep;
    struct Proto   **p;
} Proto;

typedef struct {
    struct BijouVM *vm;
    bijou_Writer writer;
    void *data;
    int status;
} DumpState;

Proto *to_proto(struct BijouVM*, BijouBlock*);
void Proto_destroy(Proto*);
int bijou_dump(struct BijouVM*, Proto*, bijou_Writer, void*);
Proto *compile_BijouBlock(struct BijouVM*, BijouBlock*);
void make_header(char* h);

#endif /* _DUMP_H_*/
