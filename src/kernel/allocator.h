#ifndef ALLOCATOR_H
#define ALLOCATOR_H

typedef struct {
  void*(*malloc)(int size);
  void*(*calloc)(int count, int size);
  void(*free)(void*);
} allocator;

#endif
