# Description

This is my own implementation of **malloc**, **free**, **realloc** and **calloc**.

#Usage

Just copy and paste **malloc.h** and **malloc.c** in your project and include **malloc.h** in your **\*.c/\*.cpp** files.

The function calls are defined in the POSIX standard:

```
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

```
