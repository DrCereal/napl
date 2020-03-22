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
#include "vector.h"

static PtrVector* symbol_table;

Buffer* text;
Buffer* rodata;

void
initialize_generate (void)
{
  symbol_table = create_ptr_vector();
  text = create_buffer();
  rodata = create_buffer();

  buffer_addstr(text, ".section .text\n");
  buffer_addstr(rodata, ".section .rodata\n");
}

void
Emit_Extern (char* name, size_t size)
{
  buffer_addstr(text, ".extern ");

  char str[size + 1];
  memcpy(str, name, size);
  str[size] = '\0';
  buffer_addstr(text, str);

  buffer_addc(text, '\n');

  //To prevent overwriting the extern function and to ensure
  //that we know it's there.
  Symbol* symbol = create_symbol(name, size, SYMBOL_FUNCTION);
  ptr_vector_push(symbol_table, (void*)symbol);
}

void
Emit_Function_Header (char* name, size_t size)
{
  char* new_name = alloc(NULL, size + 20);
  memcpy(new_name, name, size);
  sprintf(new_name + size, "\0");

  Symbol* symbol = find_symbol(symbol_table, new_name);
  if (symbol == NULL)
    {
      symbol = create_symbol(new_name, size, SYMBOL_FUNCTION);
      ptr_vector_push(symbol_table, (void*)symbol);
    }
  else
    error(-1, 0, "symbol `%s` already exists", new_name);

  buffer_addstr(text, ".global ");
  buffer_addstr(text, new_name);
  buffer_addc(text, '\n');

  buffer_addstr(text, new_name);
  buffer_addstr(text, ":\n");

  buffer_addstr(text, "pushq %rbp\n");
  buffer_addstr(text, "movq %rsp, %rbp\n");
}

void
Emit_Function_Return (int value)
{
  buffer_addstr(text, "popq %rbp\n");
  buffer_addstr(text, "movq $");
  char str[20];
  sprintf(str, "%i", value);
  buffer_addstr(text, str);
  buffer_addstr(text, ", %rax\n");

  buffer_addstr(text, "ret\n");
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

  Symbol* symbol = find_symbol(symbol_table, new_name);
  if (symbol == NULL)
    error(-1, 0, "function `%s` is not defined", new_name);

  buffer_addstr(text, "call ");
  buffer_addstr(text, new_name);
  buffer_addc(text, '\n');
}

static unsigned int current_var_reg = 0;

void
Emit_Variable_Register (void)
{
  switch (current_var_reg++)
    {
      case 0: buffer_addstr(text, "rdi"); break;
      case 1: buffer_addstr(text, "rsi"); break;
      case 2: buffer_addstr(text, "rdx"); break;
      case 3: buffer_addstr(text, "rcx"); break;
      case 4: buffer_addstr(text, "r8"); break;
      case 5: buffer_addstr(text, "r9"); break;
      default:
        error(-1, 0, "Emit_Variable(): ran out of registers");
    }
}

void
Emit_Variable_Number (int value)
{
  buffer_addstr(text, "movq $");
  char str[20];
  sprintf(str, "%i", value);
  buffer_addstr(text, str);
  buffer_addstr(text, ", %");

  Emit_Variable_Register();
  
  buffer_addc(text, '\n');
}

static unsigned int local_variable_id = 0;

void
Emit_Variable_String (char* name, size_t size)
{
  char str1[32];
  sprintf(str1, "L%i\0", local_variable_id);
  buffer_addstr(rodata, str1);
  buffer_addstr(rodata, ":\n");

  buffer_addstr(rodata, ".string \"");
  char str2[size + 1];
  memcpy(str2, name, size);
  str2[size] = '\0';
  buffer_addstr(rodata, str2);
  buffer_addstr(rodata, "\"\n");

  buffer_addstr(text, "movq $");
  buffer_addstr(text, str1);
  buffer_addstr(text, ", %");
  
  Emit_Variable_Register();

  buffer_addc(text, '\n');
}

void
Emit_Variable_Init (const char* name, int value)
{
  buffer_addstr(text, "movq $");
  char str[20];
  sprintf(str, "%i", value);
  buffer_addstr(text, str);
  buffer_addstr(text, ", %rax\n");
}

void
write_emission (void)
{
  write_buffer(rodata);
  write_buffer(text);
}
