#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdio.h>

int compile_file(FILE*, FILE*, unsigned int);

struct BijouVM;
struct BijouBlock;

void compile_header(FILE*, struct BijouVM*, struct BijouBlock*);

/* read the next char (ignores whitespace (if second arg is true)
 * skips over comments)
 */
char read_char(FILE*, int);

void expect(FILE*, char*);
#endif /* _COMPILER_H_ */
