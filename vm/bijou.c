#include "bijou.h"
#include "internal.h"
#include "vm.h"


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
        bijou_interpret();
        return usage();
}


