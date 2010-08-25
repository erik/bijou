#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "func.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define SHOULD_BE(tval, type) if((tval).tt != (type)) { \
	fprintf(stderr, "Expected type %d,"	        \
	"but got %d (%s)\n", type, (tval).tt, TValue_to_string(tval)); \
	exit(1); \
    }

/* parameters to pass to thread_call() */
typedef struct call_struct {
    BijouVM* vm;
    BijouBlock *b;
    BijouFunction *func;
    TValue *args;
    int argc;
} call_struct;


/* handle for all threads to use */
static void* thread_call(void *call_str)
{
    call_struct* cs = (call_struct *)call_str;

    BijouFunction_call(cs->vm, cs->b, *cs->func, cs->args, cs->argc);

    pthread_exit(NULL);
}

/*
 * Args -
 *	function [Closure]
 * Returns -
 *	thread   [pthread_t]
 */
int    args_thread_create = 1;
TValue func_thread_create(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    pthread_t* thread;
    int return_val;
    call_struct* cs;

    SHOULD_BE(argv[0], BIJOU_TFUNCTION);

    thread = B_ALLOC(pthread_t);
    cs   = B_ALLOC(call_struct);

    cs->vm   = vm;
    cs->b    = blk;
    cs->argc = 0;
    cs->func = argv[0].value.func;
    cs->args = NULL;

    return_val = pthread_create(thread, NULL, thread_call, (void *)cs);

    if (return_val) {
        fprintf(stderr, "Error: thread_create returned with error code: %d\n", return_val);
        exit(1);
    }

    return create_pointer(thread);
}

/*
 * Args -
 *	seconds to sleep [int]
 * Returns -
 *	[null]
 */
int    args_sleep = 1;
TValue func_sleep(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    long seconds;

    SHOULD_BE(argv[0], BIJOU_TNUMBER);

    seconds = (long)argv[0].value.n;

    pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    struct timespec ts;
    struct timeval  tp;

    gettimeofday(&tp, NULL);

    ts.tv_sec  =  tp.tv_sec;
    ts.tv_nsec =  tp.tv_usec * 1000;
    ts.tv_sec  += seconds;

    int ret = pthread_cond_timedwait(&cond, &mutex, &ts);

    if (ret == -1) {
        fprintf(stderr, "Sleep error: %s\n", strerror(errno));
        exit(1);
    }

    return create_null();

}

/*
 * Args -
 *	NONE
 * Returns -
 *	[null]
 */
int    args_thread_yield = 0;
TValue func_thread_yield(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);
    UNUSED(argv);

    int status = sched_yield();

    if (status != 0) {
        fprintf(stderr, "Error on thread_yield: %s\n", strerror(status));
        exit(1);
    }

    return create_null();
}

/*
 * Args -
 *	thread [pthread_t]
 * Returns -
 *	return val [TValue]
 */
int    args_thread_join = 1;
TValue func_thread_join(VM, BijouBlock* blk, int argc, TValue* argv)
{
    UNUSED(vm);
    UNUSED(blk);
    UNUSED(argc);

    pthread_t *thread;
    TValue* val;
    SHOULD_BE(argv[0], BIJOU_TPOINTER);

    thread = (pthread_t *)argv[0].value.pointer;
    pthread_join(*thread, (void**)&val);

    return *val;

}
