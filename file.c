/*  file.c - File i/o boiler plate
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

#include <errno.h>
#include <error.h>
#include <stdio.h>

#include "file.h"

FILE* file_read;
FILE* file_write;
const char* file_name;

int
readc (void)
{
  int c = fgetc(file_read);
  //Problem reading file. Terminate.
  if (c == EOF && ferror(file_read))
    error(-1, 0, "readc(): failed");

  return c;
}

int
unreadc (int c)
{
  return ungetc(c, file_read);
}  

int
peekc (void)
{
  int c = readc();
  if (c == EOF)
    return c;
  else
    {
      if (unreadc(c) == EOF)
        error(-1, 0, "unreadc(): failed");
      
      return c;
    }
}

void
writec (const char c)
{
  if (file_write == NULL)
    file_write = fopen("a.s", "w");

  if (file_write == NULL)
    error(-1, errno, "writec()");

  fputc(c, file_write);
}

void
writestr (const char* str)
{
  while (*str != '\0') writec(*str++);
}

void
writestrn (const char* str, size_t size)
{
  for (int i = 0; i < size; i++) writec(str[i]);
}

void
open_file (const char* string)
{
  file_name = string;

  file_read = fopen(file_name, "r");
  //Problem opening file. Terminate.
  if (file_read == NULL)
    error(-1, errno, "open_file()");
}

const char*
get_file_name (void)
{
  return file_name;
}
