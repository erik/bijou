#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdio.h>

int compile_file(FILE*, FILE*, unsigned int);

struct BijouVM;
struct BijouBlock;

void compile_header(FILE*, struct BijouVM*, struct BijouBlock*);
void compile_code(FILE*, struct BijouVM*, struct BijouBlock*);
void compile_const(FILE*, struct BijouVM*, struct BijouBlock*);

/* read the next char (ignores whitespace (if second arg is true)
 * skips over comments)
 */
char read_char(FILE*, int);

/*
 * reads next 'word' (such as '.regs', or '20')
 */
char *read_next(FILE*);

/*
 * reads all the arguments on the current line
 * returns an int array of size 3.
 * Will convert constants to proper format
 */
int *read_args(FILE*);

void expect(FILE*, char*);
#endif /* _COMPILER_H_ */
