//
//  malloc.c
//  Malloc
//
//  Created by Zoltán Sasvári on 05.06.15.
//  Copyright (c) 2015 BinaryTrasher. All rights reserved.
//

#include "malloc.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct metadata {
  int magic1;
  size_t size;
  struct metadata *next;
  struct metadata *prev;
  int free;
  int magic2;
} metadata_t;

void *base = 0;
int alloc_count = 0;
pthread_mutex_t list_lock;

#define METADATA_SIZE sizeof(struct metadata)
#define MAGIC_FREE 0xDEADDA7A
#define MAGIC_USED 0xB16B00B5
#define SEGFAULT "Segmentation fault!\n"

metadata_t *get_free_block(metadata_t **last, size_t size){
  metadata_t *current = base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
    if (!current) continue;
    if ((current->magic1 != MAGIC_USED && current->magic1 != MAGIC_FREE) || current->magic1 != current->magic2) {
      printf(SEGFAULT);
      exit(-1);
    }
  }
  return current;
}

metadata_t *get_space(metadata_t* last, size_t size) {
  metadata_t *block;
  block = sbrk(0);
  void *request = sbrk((int)(size + METADATA_SIZE));
  if ((void*)block != request) {
    printf(SEGFAULT);
    exit(-1);
  }
  if (request == (void*) -1) {
    return 0; // shit
  }

  if (last) {
    last->next = block;
    block->prev = last;
  } else {
    block->prev = NULL;
  }
  block->size = size;
  block->next = 0;
  block->free = 0;
  block->magic1 = MAGIC_USED;
  block->magic2 = MAGIC_USED;
  return block;
}

void *malloc(size_t size) {
  metadata_t *block;
  // TODO: align size?
  if (size <= 0) {
    return 0;
  }
  if (!base) { // first
    pthread_mutex_init(&list_lock, NULL);
    pthread_mutex_lock(&list_lock);
    block = get_space(0, size);
    if (!block) {
      pthread_mutex_unlock(&list_lock);
      return 0;
    }
    base = block;
//    pthread_mutex_unlock(&list_lock);
  } else {
    pthread_mutex_lock(&list_lock);
    metadata_t *last = base;
    block = get_free_block(&last, size);
    if (!block) {
      block = get_space(last, size);
      if (!block) {
        pthread_mutex_unlock(&list_lock);
        return 0;
      }
    } else {
      // TODO: split
      block->free = 0;
      block->magic1 = MAGIC_USED;
      block->magic2 = MAGIC_USED;
    }
  }
  alloc_count++;
  pthread_mutex_unlock(&list_lock);
  return (block+1);
}

metadata_t *get_block(void *ptr){
  metadata_t* ret_val =  ((metadata_t*)ptr)-1;
  if ((ret_val->magic1 != MAGIC_USED && ret_val->magic1 != MAGIC_FREE) || ret_val->magic1 != ret_val->magic2)
  {
    printf(SEGFAULT);
    exit(-1);
  }
  return ret_val;
}

void merge_block(metadata_t*block){
  if (block->next && block->next->free){
    block->size += METADATA_SIZE + block->next->size;
    block->next = block->next->next;
  }
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }
  pthread_mutex_lock(&list_lock);
  metadata_t* block_ptr = get_block(ptr);
  merge_block(block_ptr);

  if (block_ptr->free != 0 || block_ptr->magic1 != MAGIC_USED || block_ptr->magic1 != block_ptr->magic2) {
    printf(SEGFAULT);
    exit(-1);
  }
  alloc_count--;
  if (alloc_count == 0){
    brk(base);
    base = 0;
  } else if (block_ptr->next == NULL){
    //printf("free\n");
    block_ptr->prev->next = NULL;
    brk(block_ptr);
  } else {
    block_ptr->free = 1;
    block_ptr->magic1 = MAGIC_FREE;
    block_ptr->magic2 = MAGIC_FREE;
  }
  pthread_mutex_unlock(&list_lock);
}

void *realloc(void *ptr, size_t size) {
  if (!ptr) {
    return malloc(size);
  }
  pthread_mutex_lock(&list_lock);
  metadata_t *block_ptr = get_block(ptr);
  if (block_ptr->size >= size) {
    pthread_mutex_unlock(&list_lock);
    return ptr;
  }
  void *new_ptr;
  pthread_mutex_unlock(&list_lock);
  new_ptr = malloc(size);
  if (!new_ptr) {
    return 0;
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  free(ptr);
  return new_ptr;
}

void *calloc(size_t nelem, size_t elsize) {
  if (nelem == 0 || elsize == 0) {
    return 0;
  }
  void *ptr = malloc(nelem*elsize);
  memset(ptr, 0, nelem*elsize);
  alloc_count++;
  return ptr;
}