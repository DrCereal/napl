#ifndef PARSER_H
#define PARSER_H

enum operator_type
{
  OPERATOR_ADD,
  OPERATOR_SUBTRACT,
  OPERATOR_MULTIPLY,
  OPERATOR_DIVIDE,
};

#include <stdbool.h>

void parse(void);
bool parser_failed(void);

#endif
