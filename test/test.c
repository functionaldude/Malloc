//
//  test.c
//  Malloc
//
//  Created by Zoltán Sasvári on 25.06.15.
//  Copyright (c) 2015 BinaryTrasher. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "../malloc.h"

pthread_mutex_t out_lock;

void * test(){
  int* data[6];
  data[0] = (int*)malloc(5*sizeof(int));
  data[1] = (int*)malloc(20*sizeof(int));
  data[2] = (int*)malloc(40*sizeof(int));
  data[3] = (int*)malloc(100*sizeof(int));
  data[4] = (int*)malloc(500*sizeof(int));
  data[5] = (int*)malloc(100*sizeof(int));
  int i = 0;
  for(i = 0;i<40;i++){
    data[2][i] = 0x99999999;
  }
  for(i = 0;i<500;i++){
    data[4][i] = 666;
  }
  free(data[0]);
  free(data[1]);
  free(data[3]);
  for(i = 0;i<40;i++){
    assert(data[2][i] == 0x99999999);
  }
  for(i = 0;i<500;i++){
    assert(data[4][i] == 666);
  }
  data[0] = (int*)malloc(25*sizeof(int));
  for(i = 0;i<25;i++){
    data[0][i] = 777;
  }
  for(i = 0;i<25;i++){
    assert(data[0][i] == 777);
  }
  free(data[0]);
  free(data[2]);
  free(data[4]);
  free(data[5]);
  pthread_mutex_lock(&out_lock);
  printf("Passed malloc-free!\n");
  pthread_mutex_unlock(&out_lock);


  data[0] = (int*)calloc(5,sizeof(int));
  data[1] = (int*)calloc(20,sizeof(int));
  data[2] = (int*)calloc(40,sizeof(int));
  data[3] = (int*)calloc(100,sizeof(int));
  data[4] = (int*)calloc(500,sizeof(int));
  data[5] = (int*)calloc(100,sizeof(int));
  i = 0;
  for(i = 0;i<40;i++){
    data[2][i] = 0x99999999;
  }
  for(i = 0;i<500;i++){
    data[4][i] = 666;
  }
  free(data[0]);
  free(data[1]);
  free(data[3]);
  for(i = 0;i<40;i++){
    assert(data[2][i] == 0x99999999);
  }
  for(i = 0;i<500;i++){
    assert(data[4][i] == 666);
  }
  data[0] = (int*)calloc(25,sizeof(int));
  free(data[0]);
  free(data[2]);
  free(data[4]);
  free(data[5]);
  pthread_mutex_lock(&out_lock);
  printf("Passed calloc-free!\n");
  pthread_mutex_unlock(&out_lock);

  data[2] = (int*)calloc(40,sizeof(int));
  for(i = 0;i<40;i++){
    data[2][i] = 0x99999999;
  }
  data[2] = (int*)realloc(data[2], 80*sizeof(int));
  for(i = 40;i<80;i++){
    data[2][i] = 0x99999999;
  }
  for(i = 0;i<80;i++){
    assert(data[2][i] == 0x99999999);
  }
  free(data[2]);
  pthread_mutex_lock(&out_lock);
  printf("Passed realloc-free!\n");
  pthread_mutex_unlock(&out_lock);

  return NULL;
}

#define THREAD_NUM 5
int main(){
  pthread_mutex_init(&out_lock, NULL);
  printf("Malloc test: \n");
  printf("Single threaded:\n");
  test();

  printf("Multi threaded:\n");
  pthread_t thread[THREAD_NUM];
  for (int i = 0; i<THREAD_NUM; ++i) {
    pthread_create(&thread[i], NULL, test, NULL);
  }
  for (int i = 0; i<THREAD_NUM; ++i) {
    pthread_join(thread[i], NULL);
  }


  printf("All tests passed!\n");
  return 0;
}