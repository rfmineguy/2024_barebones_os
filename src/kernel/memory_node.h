#ifndef MEMORY_NODE_H
#define MEMORY_NODE_H
#include "../stdlib/stdbool.h"

typedef struct {
  bool free;
  void *begin, *end;
} memory_node;

#endif
