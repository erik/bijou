#ifndef _LOAD_H_
#define _LOAD_H_

typedef struct LoadState {
    struct BijouVM *vm;
    BijouBlock *block;
    bijou_Reader *reader;
    const char *name;
} LoadState;

struct Proto* bijou_load(VM, BijouBlock*, bijou_Reader*, const char*);

#endif /* _LOAD_H_ */
