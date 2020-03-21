/*  parser.c - The parser.
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

#include <error.h>
#include <stdio.h>

#include "file.h"
#include "generate.h"
#include "lexer.h"
#include "parser.h"

static bool failed = false;

static void
eat_new_lines (void)
{
  Token* new_line = get_token();
  while (new_line->token_type == TOKEN_NEWLINE)
    {
      delete_token(new_line);
      new_line = get_token();
    }
  unget_token(new_line);
}

//<operator> ::= `+` | `-` | `*` | `/`
int operator_evaluated;
bool
Operator (void)
{
  Token* token = peek_token();
  switch (token->token_type)
    {
      case TOKEN_PLUS: operator_evaluated = OPERATOR_ADD; break;
      case TOKEN_MINUS: operator_evaluated = OPERATOR_SUBTRACT; break;
      case TOKEN_ASTERISK: operator_evaluated = OPERATOR_MULTIPLY; break;
      case TOKEN_SLASH: operator_evaluated = OPERATOR_DIVIDE; break;
      default:
        return false;
    }

  delete_token(get_token());
  return true;
}

//<expression> ::= <Number> <operator> <expression>
//               | `(` <expression> `)` [<operator> <expression>]
//               | <Number>
int expression_evaluated;
bool
Expression (void)
{
  bool failed_ = false;

  Token* token1 = peek_token();
  if (token1->token_type != TOKEN_NUMBER &&
      token1->token_type != TOKEN_PAREN_OPEN)
    return false;
  get_token();

  if (token1->token_type == TOKEN_NUMBER)
    {
      if (!Operator())
        {
          expression_evaluated = token1->number;
          
          delete_token(token1);
          return true;
        }
      int operator = operator_evaluated;
      
      if (!Expression())
        {
          error_at_line(0, 0, get_file_name(), token1->line_number, \
            "expected expression after operator");
          failed = true;
          failed_ = true;
        }

      if (!failed_)
        {
          switch (operator)
            {
              case OPERATOR_ADD:
                expression_evaluated += token1->number;
                break;
              case OPERATOR_MULTIPLY:
                expression_evaluated *= token1->number;
                break;
              case OPERATOR_SUBTRACT:
                expression_evaluated = token1->number - expression_evaluated;
                break;
              case OPERATOR_DIVIDE:
                expression_evaluated = token1->number / expression_evaluated;
                break;
            }
        }

      delete_token(token1);
      return true;
    }
  else
    {
      if (!Expression())
        {
          error_at_line(0, 0, get_file_name(), token1->line_number, \
            "expected expression after `(` token");

          failed = true;
          failed_ = true;
        }
      int expression1 = expression_evaluated;

      Token* token2 = get_token();
      if (token2->token_type != TOKEN_PAREN_CLOSE)
        {
          error_at_line(0, 0, get_file_name(), token1->line_number, \
            "expected `)` token after expression");
          failed = true;
          failed_ = true;
        }

      if (!Operator())
        {
          delete_token(token1);
          delete_token(token2);
          return true;
        }

      int operator = operator_evaluated;
      if (!Expression())
        {
          error_at_line(0, 0, get_file_name(), token1->line_number, \
            "expected expression after operator token");
          failed = true;
          failed_ = true;
        }

      if (!failed_)
        {
          switch (operator)
            {
              case OPERATOR_ADD:
                expression_evaluated += expression1;
                break;
              case OPERATOR_MULTIPLY:
                expression_evaluated *= expression1;
                break;
              case OPERATOR_SUBTRACT:
                expression_evaluated = expression1 - expression_evaluated;
                break;
              case OPERATOR_DIVIDE:
                expression_evaluated = expression1 / expression_evaluated;
                break;
            }
        }

      delete_token(token1);
      delete_token(token2);
      return true;
    }
}

//<variable-initialization> ::= <Identifier> `=` <expression>
bool
VariableInitialization (void)
{
  Token* token1 = peek_token();
  if (token1->token_type != TOKEN_IDENTIFIER)
    return false;
  get_token();

  Token* token2 = peek_token();
  if (token2->token_type != TOKEN_EQUAL)
    {
      unget_token(token1);
      return false;
    }
  get_token();

  if (!Expression())
    {
      error_at_line(0, 0, get_file_name(), token2->line_number, \
        "expected expression after `=`");
      failed = true;
    }

  //Perform necessary operations
  if (!failed)
    {
      Emit_Variable_Init(token1->data, expression_evaluated);
    } 

  delete_token(token1);
  delete_token(token2);
  return true;
}

//<function-call> ::= <Identifier> `()`
bool
FunctionCall (void)
{
  Token* token1 = peek_token();
  if (token1->token_type != TOKEN_IDENTIFIER)
    return false;
  get_token();

  Token* token2 = peek_token();
  if (token2->token_type != TOKEN_PAREN_OPEN)
    {
      unget_token(token1);
      return false;
    }
  get_token();

  Token* token3 = get_token();
  if (token3->token_type != TOKEN_PAREN_CLOSE)
    {
      error_at_line(0, 0, get_file_name(), token2->line_number, \
        "expected `)` token after `(` token");
      failed = true;
    }

  if (!failed)
    Emit_Function_Call(token1->data, token1->data_length);

  delete_token(token3);
  delete_token(token2);
  delete_token(token1);
  return true;
}

//<return-statement> ::= `return` <Number>
bool
ReturnStatement (void)
{
  Token* token1 = peek_token();
  if (token1->token_type != TOKEN_KEYWORD)
    return false;
  
  if (token1->data_length != 6)
    return false;
  else if (memcmp(token1->data, "return", 6) != 0)
    return false;
  get_token();

  Token* token2 = get_token();
  if (token2->token_type != TOKEN_NUMBER)
    {
      error_at_line(0, 0, get_file_name(), token1->line_number, \
        "expected identifier after token `return`");

      failed = true;
    }

  if (!failed)
    Emit_Return(token2->number);

  delete_token(token2);
  delete_token(token1);
  return true;
}

//<statement> ::= <variable-initialization>
//              | <function-call>
//              | <return-statement>
bool
Statement (void)
{
  if (VariableInitialization()) return true;
  if (FunctionCall()) return true;
  if (ReturnStatement()) return true;
  return false;
}

//<compound-statement> ::= `{` {<statement>}* `}`
bool
CompoundStatement (void)
{
  Token* token1 = peek_token();
  if (token1->token_type != TOKEN_CURLY_OPEN)
    return false;
  get_token();

  eat_new_lines();

  while (Statement())
    {
      Token* new_line = get_token();
      if (new_line->token_type != TOKEN_NEWLINE)
        {
          error_at_line(0, 0, get_file_name(), new_line->line_number, \
            "expected new line after statement");
          failed = true;
        }

      eat_new_lines();
    }

  eat_new_lines();

  Token* token2 = get_token();
  if (token2->token_type != TOKEN_CURLY_CLOSE)
    {
      printf("%s\n", token2->data);
      error_at_line(0, 0, get_file_name(), token1->line_number, \
        "expected closing `}` token");
      failed = true;
    }

  delete_token(token1);
  delete_token(token2);
  return true;
}

//<function-definition> ::= `func` <Identifier> `()`
//                          {<compound-statement>}+
bool
FunctionDefinition (void)
{
  Token* token1 = peek_token();

  if (token1->token_type != TOKEN_KEYWORD)
    return false;
  if (memcmp(token1->data, "func", 4) != 0)
    return false;
  get_token();

  Token* token2 = get_token();
  if (token2->token_type != TOKEN_IDENTIFIER)
    {
      error_at_line(0, 0, get_file_name(), token1->line_number, \
        "expected identifier after token `func`");
      failed = true;
    }

  Token* token3 = get_token();
  Token* token4 = get_token();
  if (token3->token_type != TOKEN_PAREN_OPEN)
    {
      error_at_line(0, 0, get_file_name(), token1->line_number, \
        "expected `(` token after `func` token");
      failed = true;
    }
  else if (token4->token_type != TOKEN_PAREN_CLOSE)
    {
      error_at_line(0, 0, get_file_name(), token1->line_number, \
        "expected `)` token after `(` token");
      failed = true;
    }

  eat_new_lines();

  if (!failed)
    {
      Emit_Function_Header(token2->data, token2->data_length);

      CompoundStatement();

      Emit_Function_Return();
    }

  delete_token(token1);
  delete_token(token2);
  delete_token(token3);
  delete_token(token4);
  return true;
}

//<preprocessor> ::= `import` <Identifier>
bool
PreProcessor (void)
{
  Token* token1 = peek_token();
  if (token1->token_type != TOKEN_KEYWORD)
    return false;
  
  if (token1->data_length != 6)
    return false;
  else if (memcmp(token1->data, "import", 6) != 0)
    return false;
  get_token();

  Token* token2 = get_token();
  if (token2->token_type != TOKEN_IDENTIFIER)
    {
      error_at_line(0, 0, get_file_name(), token1->line_number, \
        "expected identifier after token `import`");
      failed = true;
    }

  if (!failed)
    {
      if (memcmp(token2->data, "print", token2->data_length) == 0)
        Emit_Extern(token2->data, token2->data_length);
    }

  delete_token(token2);
  delete_token(token1);
  return true;
}

void
parse (void)
{
  while (PreProcessor())
    eat_new_lines();
  while (FunctionDefinition())
    eat_new_lines();

  Token* test = get_token();
  if (test->token_type != TOKEN_NEWLINE && test->token_type != TOKEN_EOF)
    {
      printf("%i\n", test->token_type);
      char str[test->data_length + 1];
      memcpy(str, test->data, test->data_length);
      str[test->data_length] = '\0';

      error_at_line(-1, 0, get_file_name(), test->line_number, \
        "got garbage at end of file `%s`", str);
    }
}

bool
parser_failed (void)
{
  return failed;
}
