/*  symbol.c - Everything to do with symbols
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

#include <stdio.h>
#include <string.h>

#include "symbol.h"
#include "vector.h"

Symbol*
create_symbol (char* data, size_t size, 
               int symbol_type)
{
  Symbol* symbol = alloc(NULL, sizeof(Symbol));
  symbol->data = alloc(NULL, size);
  memcpy(symbol->data, data, size);
  symbol->data_length = size;
  symbol->symbol_type = symbol_type;

  return symbol;
}

Symbol*
find_symbol (PtrVector* table, const char* data)
{
  Symbol* symbol;
  for (int i = 0; i < table->vector_size; i++)
    {
      symbol = (Symbol*)table->ptr[i];
      if (strcmp(data, symbol->data) != 0)
        continue;

      return symbol;
    }

  return NULL;
}
