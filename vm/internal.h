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

#if defined __linux || __APPLE__ & __MACH__
#include <dlfcn.h>
#define LIB_LOAD(file)    dlopen(file, RTLD_NOW | RTLD_GLOBAL)
#define LIB_READ(h, func) dlsym(h, func)
#define LIB_CLOSE(func)   dlclose(func)
#define LIB_ERROR         dlerror()
#else
#define LIB_LOAD(file) #error "Dynamic loading isn't available on this platform yet"
#define LIB_READ(h, f) #error "Dynamic loading isn't available on this platform yet"
#define LIB_CLOSE(fun) #error "Dynamic loading isn't available on this platform yet"
#define LIB_ERROR      #error "Dynamic loading isn't available on this platform yet"
#endif



#define check_exp(check, exp)   (bijou_assert(check), exp)

#endif /* _INTERNAL_H_ */
