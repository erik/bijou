/* Various functions to create and manipulate
 * structs of type BijouBlock
*/
#include "bopcodes.h"
#include "internal.h"
#include "bijou.h"


BijouBlock *BijouBlock_new()
{
        BijouBlock *b = B_ALLOC(BijouBlock);
        kv_init(b->k);
        kv_init(b->code);
        kv_init(b->locals);
        kv_init(b->strings);
        b->regc = 0;

        return b;
}

/* push a TValue into constants of block
 * returns index
 */
int BijouBlock_push_const(BijouBlock *b, TValue v)
{
        kv_push(TValue, b->k, v);
        return kv_size(b->k) - 1;
}

/* push a TValue into local stack, returning
 * index of local
 */
int BijouBlock_push_local(BijouBlock *b, TValue v)
{
        kv_push(TValue, b->upvals, v);
        return kv_size(b->k) - 1;
}

/* search for constant matching TValue v
 * return index of match, or -1 if not found
 */
int BijouBlock_find_const(BijouBlock *b, TValue v)
{
        size_t i;
        for (i = 0; i < kv_size(b->k); i++) {
                if (TValue_equal(v, kv_A(b->k, i)))
                        return i;
        }
        return -1;
}

/* search for local matching TValue v
 * return index of match, or -1 if not found
 */
int BijouBlock_find_local(BijouBlock *b, TValue v)
{
        size_t i;
        for (i = 0; i < kv_size(b->k); i++) {
                if (TValue_equal(v, kv_A(b->k, i)))
                        return i;
        }
        return -1;
}
