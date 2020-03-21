#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <string.h>

#define char_token(c,t) \
token->data[0] = c; token->token_type = t; return token

#define check_for_keyword(key,s) \
if (memcmp(token->data, key, s) == 0) \
  token->token_type = TOKEN_KEYWORD;

enum token_type
{
  INVALID_TOKEN = -1,

  TOKEN_ASTERISK,
  TOKEN_CURLY_OPEN,
  TOKEN_CURLY_CLOSE,
  TOKEN_EOF,
  TOKEN_EQUAL,
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_MINUS,
  TOKEN_NEWLINE,
  TOKEN_NUMBER,
  TOKEN_PAREN_OPEN,
  TOKEN_PAREN_CLOSE,
  TOKEN_PLUS,
  TOKEN_SLASH,
};

typedef struct
{
  enum token_type token_type;
  int number;
  size_t line_number;
  size_t data_length;
  char* data;
} Token;

void initialize_lexer();

Token* get_token(void);
void unget_token(Token* token);
Token* peek_token(void);
void delete_token(Token* token);

bool lexer_failed(void);
#endif
