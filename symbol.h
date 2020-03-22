#ifndef SYMBOL_H
#define SYMBOL_H

#include "vector.h"

enum symbol_type
{
  SYMBOL_FUNCTION,
  SYMBOL_VARIABLE,
};

typedef struct
{
  char* data;
  int data_length;
  int symbol_type;
} Symbol;

Symbol* create_symbol(char* data, size_t size, int symbol_type);
Symbol* find_symbol(PtrVector* vector, const char* data);

#endif
