#ifndef _VM_H_
#define _VM_H_

/* TODO: write me! */
typedef struct BijouVM {
    size_t numglobals;
    TValue* globals;
    kvec_t(BijouFunction) functions;

} BijouVM;

BijouVM *BijouVM_new(size_t);
void BijouVM_destroy(BijouVM*);
int BijouVM_push_global(BijouVM*, TValue);
int BijouVM_find_global(BijouVM*, TValue);
TValue bijou_interpret(VM, BijouFrame *f, BijouBlock *b, int start, int argc, TValue argv[] /*Closure *closure*/);

#endif /* _VM_H_ */
