#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct
{
  void** ptr;
  size_t vector_size;
  size_t vector_max_size;
} PtrVector;

void* alloc(void* old_ptr, size_t size);

PtrVector* create_ptr_vector();
void delete_ptr_vector(PtrVector* vector);
void ptr_vector_push(PtrVector* vector, void* item);
void* ptr_vector_pop(PtrVector* vector);

#endif
