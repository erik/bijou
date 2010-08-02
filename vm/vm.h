#ifndef _VM_H_
#define _VM_H_

void bijou_interpret(VM, BijouFrame *f, BijouBlock *b, int start, int argc, TValue argv[] /*Closure *closure*/);

#endif /* _VM_H_ */
