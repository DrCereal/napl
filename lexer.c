/*  lexer.c - Gets lexemes for compiler.
    Copyright (C) 2020 Jakob Brothwell

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ctype.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "lexer.h"
#include "vector.h"

static PtrVector* vector;

static size_t line_number = 1;
static bool failed = false;

void
initialize_lexer ()
{
  vector = create_ptr_vector();
}

Token*
get_token (void)
{
  Token* token = (Token*)ptr_vector_pop(vector);
  if (token != NULL)
    return token;

  token = alloc(NULL, sizeof(Token));

  token->token_type = INVALID_TOKEN;
  token->data_length = 1;
  token->data = alloc(NULL, 1);

  //Read character stream and get lexeme with initial guess of type
  //Remove unnecessary whitespace.
  int c;
restart:
  do
    c = readc();
  while (isspace(c) && c != '\n');

  token->line_number = line_number;

  switch (c)
    {
      case EOF: char_token(EOF, TOKEN_EOF);
      case '=': char_token('=', TOKEN_EQUAL);
      case '+': char_token('+', TOKEN_PLUS);
      case '-': char_token('-', TOKEN_MINUS);
      case '*': char_token('*', TOKEN_ASTERISK);
      case '(': char_token('(', TOKEN_PAREN_OPEN);
      case ')': char_token(')', TOKEN_PAREN_CLOSE);
      case '{': char_token('{', TOKEN_CURLY_OPEN);
      case '}': char_token('}', TOKEN_CURLY_CLOSE);
      case ',': char_token(',', TOKEN_COMMA);
      case '\n': line_number++; char_token('\n', TOKEN_NEWLINE);
      case '/':
        if (peekc() == '/')
          {
            while ((c = readc()) != '\n');
            unreadc(c);

            goto restart;
          }
        else
          char_token('/', TOKEN_SLASH);
      default:
        if (isdigit(c))
          {
            int integer = 0;

            char* data = alloc(NULL, 64);
            int i = 0;

            while (isdigit(c))
              {
                integer *= 10;
                integer += c - '0';

                if (i % 64 == 0)
                  data = alloc(data, i + 64);
                data[i++] = c;
                
                c = readc();
              }

            //TODO: String of chars
            if (isalpha(c) || c == '_')
              {
                error_at_line(0, 0, get_file_name(), line_number, \
                "unexpected garbage '%c' in integer '%i'", c, integer);
                failed = true;
                return token;
              }
            
            unreadc(c);
            token->number = integer;
            token->token_type = TOKEN_NUMBER;
            token->data = data;
            return token;
          }
        else if (isalpha(c) || c == '_')
          {
            char* data = alloc(NULL, 64);

            int i = 0;
            for (;; i++)
              {
                if (i % 64 == 0)
                  data = alloc(data, i + 64);

                data[i] = c;
                c = readc();
                if (!isalnum(c) && c != '_')
                  break;
              }
            unreadc(c);

            token->token_type = TOKEN_IDENTIFIER;
            token->data = data;
            token->data_length = ++i;
            break;
          }
        else
          error_at_line(-1, 0, get_file_name(), line_number, \
          "unknown character '%c'", c);
    }

  //If lexeme is an identifier, check for keyword.
  check_for_keyword("func", 4);
  check_for_keyword("import", 6);
  check_for_keyword("return", 6);
  check_for_keyword("var", 3);

  return token;
}

void
unget_token (Token* token)
{
  ptr_vector_push(vector, token);
}

Token*
peek_token (void)
{
  Token* token = get_token();
  unget_token(token);

  return token;
}

void
delete_token (Token* token)
{
  if (token != NULL)
    {
      if (token->data != NULL)
        free(token->data); 
      free(token);
    }
}

bool
lexer_failed ()
{
  return failed;
}
