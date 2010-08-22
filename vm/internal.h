/* Copyright 2010 Erik Price */

#ifndef _INTERNAL_H_
#define _INTERNAL_H_

/* Gives the compiler some hints for branch cases
 * (stolen from TinyRb (which in turn stole it from
 * Rubinius))
*/
#ifdef __GNUC__
#define likely(x)         __builtin_expect((long int)(x), 1)
#define unlikely(c)       __builtin_expect((long int)(x), 0)
#else
#define likely(x)         (x)
#define unlikely(x)       (x)
#endif

#define B_ALLOC(T)        ((T *)B_MALLOC(sizeof(T)))
#define B_ALLOCN(T, N)    ((T *)B_MALLOC(sizeof(T) * (N)))

#ifdef DEBUG
#define bijou_assert(x)   assert(x)
#else
#define bijou_assert(x)   ((void) 0)
#endif

#ifdef __linux
#include <dlfcn.h>
#define LIB_LOAD(file)    dlopen(file, RTLD_LAZY)
#define LIB_READ(h, func) dlsym(h, func)
#define LIB_CLOSE(func)   dlclose(func)
#define LIB_ERROR         dlerror()
#else
#define LIB_LOAD(x, y) { fprintf(stderr, "Dynamic loading isn't yet supported on your platform\n"); exit(1); }
#define LIB_READ(x, y) { fprintf(stderr, "Dynamic loading isn't yet supported on your platform\n"); exit(1); }
#define LIB_CLOSE(x)   { fprintf(stderr, "Dynamic loading isn't yet supported on your platform\n"); exit(1); }
#define LIB_ERROR      { fprintf(stderr, "Dynamic loading isn't yet supported on your platform\n"); exit(1); }
#endif



#define check_exp(check, exp)   (bijou_assert(check), exp)

#endif /* _INTERNAL_H_ */
