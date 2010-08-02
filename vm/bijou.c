#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "config.h"


int usage(void)
{
        printf("Usage: \n");
        return 1;
}


int main(int argc, char ** argv)
{
        UNUSED(argc);
        UNUSED(argv);
        printf("SIG: %s\n", BIJOU_SIGNATURE);

        struct BijouVM a;
        a.temp = 0;

        BijouBlock *b = BijouBlock_new();
        BijouBlock_push_const(b, create_bijou_Number(10));
        BijouBlock_push_const(b, create_bijou_Number(12));
        BijouBlock_push_const(b, create_bijou_Number(13));

        BijouFrame frame;

        bijou_interpret(&a, &frame, b, 0, 0, NULL );
        return usage();
}


