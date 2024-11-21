// launch the kernel in a different mode depending on what mode it is launched in

#ifdef KERNEL_TESTING
#include "kernel_test.c"
#else
#include "kernel_no_test.c"
#endif
