/* Copyright 2010 Erik Price */

#include "const.h"
#include "bijou.h"
#include "internal.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>

void setup_internal_constants(VM)
{
    TValue t;

    /* version number */
    t = create_bijou_Number(BIJOU_VERSION);
    BijouVM_push_constant(vm, t);

    /* major version */
    t = create_bijou_Number((BIJOU_VERSION & 0xF0) >> 4);
    BijouVM_push_constant(vm, t);

    /* minor version */
    t = create_bijou_Number(BIJOU_VERSION & 0x0F);
    BijouVM_push_constant(vm, t);

    /* stdin */
    t = create_pointer((void*) stdin);
    BijouVM_push_constant(vm, t);

    /* stdout */
    t = create_pointer((void*) stdout);
    BijouVM_push_constant(vm, t);

    /* stderr */
    t = create_pointer((void*) stderr);
    BijouVM_push_constant(vm, t);
}
