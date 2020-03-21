/*  generate.c - Assembly code generation
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
#include <string.h>

#include "file.h"
#include "generate.h"
#include "symbol.h"

static PtrVector* symbol_table;

void
initialize_generate (void)
{
  symbol_table = create_ptr_vector();
}

void
Emit_Header (void)
{
  writestr(".section .text\n");
}

void
Emit_Extern (char* name, size_t size)
{
  writestr(".extern ");
  writestrn(name, size);
  writec('\n');

  //Make sure that we don't overwrite the extern function or
  //don't know that it's there.
  Symbol* symbol = create_symbol(name, size, SYMBOL_FUNCTION);
  ptr_vector_push(symbol_table, (void*)symbol);
}

void
Emit_Function_Header (char* name, size_t size)
{
  char* new_name = alloc(NULL, size + 20);
  memcpy(new_name, name, size);
  sprintf(new_name + size, "\0");

  Symbol* symbol = find_symbol(symbol_table, new_name, size);
  if (symbol == NULL)
    {
      symbol = create_symbol(new_name, size, SYMBOL_FUNCTION);
      ptr_vector_push(symbol_table, (void*)symbol);
    }
  else
    error(-1, 0, "symbol `%s` already exists", new_name);

  writestr(".global "); 

  writestr(new_name);
  writec('\n');

  writestr(new_name);
  writestr(":\n");
}

void
Emit_Function_Return (void)
{
  writestr("ret\n");
}

void
Emit_Function_Call (char* name, size_t size, size_t var_count)
{
  char* new_name = alloc(NULL, size + 20);
  memcpy(new_name, name, size);
  if (var_count != 0)
    sprintf(new_name + size, "%i\0", var_count);
  else
    sprintf(new_name + size, "\0");

  Symbol* symbol = find_symbol(symbol_table, new_name, size);
  if (symbol == NULL)
    error(-1, 0, "function `%s` is not defined", new_name);

  writestr("call ");
  writestr(new_name);

  writec('\n');
}

void
Emit_Return (int value)
{
  writestr("movq $");

  char str[20];
  sprintf(str, "%i", value);
  writestr(str);

  writestr(", %rax\n");  

  writestr("ret\n");
}

static unsigned int current_var_reg = 0;

void
Emit_Variable (int value)
{
  writestr("movq $");

  char str[20];
  sprintf(str, "%i", value);
  writestr(str);

  writestr(", %");

  switch (current_var_reg++)
    {
      case 0: writestr("rdi"); break;
      case 1: writestr("rsi"); break;
      case 2: writestr("rdx"); break;
      case 3: writestr("rcx"); break;
      case 4: writestr("r8"); break;
      case 5: writestr("r9"); break;
      default:
        error(-1, 0, "Emit_Variable(): ran out of registers");
    }
  writec('\n');
}

void
Emit_Variable_Init (const char* name, int value)
{
  writestr("movq $");

  char str[20];
  sprintf(str, "%i", value);
  writestr(str);
  
  writestr(", %rax\n");
}
