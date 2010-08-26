/* Copyright 2010 Erik Price */

#ifndef _LOAD_H_
#define _LOAD_H_

typedef struct LoadState {
    struct BijouVM *vm;
    BijouBlock *block;
    bijou_Reader reader;
    const char *name;
    FILE* fp;
} LoadState;

struct Proto* bijou_load(VM, BijouBlock*, bijou_Reader, const char*, FILE*);
struct BijouBlock* proto_to_block(VM, struct Proto*);

#endif /* _LOAD_H_ */
