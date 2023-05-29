#ifndef __BOX_H__
#define __BOX_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *box(void *object, size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        exit(1);
    }
    memcpy(ptr, object, size);
    return ptr;
}

#define BOX(x) box(&x, sizeof(x))

#endif
