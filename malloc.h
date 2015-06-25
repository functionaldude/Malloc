//
//  malloc.h
//  Malloc
//
//  Created by Zoltán Sasvári on 05.06.15.
//  Copyright (c) 2015 BinaryTrasher. All rights reserved.
//

#ifndef __Malloc__malloc__
#define __Malloc__malloc__

#include <stddef.h>

void *malloc(size_t size);

void *calloc(size_t nmemb, size_t size);

void *realloc(void *ptr, size_t size);

void free(void *ptr);

#endif /* defined(__Malloc__malloc__) */
