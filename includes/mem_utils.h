#pragma once

#include <stdlib.h>
#include <stdio.h>

#define allocate_array_2d(pointer, size1, size2)\
    if ( ( malloc(size1 * size2 * sizeof(**pointer)) ) == NULL )     \
    {                                                                           \
        fprintf(stderr, "error: cannot malloc()! Exiting!\n") ;               \
        fprintf(stderr, "error: file=`%s`, line=%d\n", __FILE__, __LINE__ ) ; \
        exit(0);                                                            \
    }




    