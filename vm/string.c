#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include <string.h>


/* create a new BijouString from the given char* */
BijouString BijouString_new(char * str)
{
    BijouString bs;

    if (str == NULL)
        bs.ptr = NULL;
    else {
        bs.ptr = B_MALLOC(strlen(str));
        strcpy(bs.ptr, str);
    }
    bs.len = str == NULL ? 0 : strlen(str);
    return bs;
}

/* create a new BijouString from the given char* */
/* copies 'from' to the end of 'to' */
BijouString BijouString_cat(BijouString to, BijouString from)
{
    BijouString ret;
    char * cat = B_MALLOC(sizeof(char) * (to.len + from.len));
    memcpy(cat, to.ptr, strlen(to.ptr));
    memcpy(cat + strlen(to.ptr), from.ptr, strlen(from.ptr) + 1);
    ret.ptr = cat;
    ret.len = strlen(cat);
    return ret;
}

/* return the string length of a BijouString */
int BijouString_len(BijouString b)
{
    return strlen(b.ptr);
}

/* Returns a substring of 'str' starting at 'begin'
 * with a length of 'length'
 */
BijouString BijouString_substr(BijouString str, int begin, int length)
{
    BijouString ret;
    char * to = B_MALLOC(sizeof(char) * length);

    strncpy(to, str.ptr + begin, length);
    ret.ptr = to;
    ret.len = strlen(to);
    return ret;
}


/* returns 1 if two strings are equal, 0 otherwise */
int BijouString_equal(BijouString one, BijouString two)
{
    return one.len == two.len && (strcmp(one.ptr, two.ptr) == 0);
}

/* return a char * from a BijouString */
char *BijouString_to_cstring(BijouString x)
{
    return x.ptr;
}
