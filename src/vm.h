/* Copyright 2010 Erik Price */

#ifndef _VM_H_
#define _VM_H_

typedef struct BijouVM {
    size_t numglobals;
    TValue* globals;

    kvec_t(TValue) constants;           /* built in, globally available constants */
    kvec_t(BijouFunction *) functions; 	/* built in, globally available functions */
    kvec_t(void *) libs; 		/* names of dynamically linked libs */

} BijouVM;

BijouVM *BijouVM_new(size_t);
void BijouVM_dump_functions(BijouVM*);
void BijouVM_dump_constants(BijouVM*);
void BijouVM_destroy(BijouVM*);
int BijouVM_find_global(BijouVM*, TValue);
int BijouVM_push_function(BijouVM*, BijouFunction*);
int BijouVM_push_constant(BijouVM*, TValue);
int BijouVM_push_lib(BijouVM*, void*);
TValue bijou_interpret(VM, BijouFrame *f, BijouBlock *b, int start, int argc, TValue argv[] /*Closure *closure*/);

#endif /* _VM_H_ */
