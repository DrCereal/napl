/*  vector.c - memory
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
#include <stdlib.h>

#include "vector.h"

//A malloc() and realloc() wrapper.
void*
alloc (void* old_ptr, size_t size)
{
  void* ptr;
  if (old_ptr == NULL)
    ptr = malloc(size);
  else
    ptr = realloc(old_ptr, size);

  if (ptr == NULL)
    error(-1, 0, "%salloc(): failed", old_ptr == NULL ? "m" : "re");
  return ptr;
}

PtrVector*
create_ptr_vector ()
{
  PtrVector* vector = alloc(NULL, sizeof(PtrVector));

  vector->ptr = NULL;
  vector->vector_size = 0;
  vector->vector_max_size = 0;

  return vector;
}

void
delete_ptr_vector (PtrVector* vector)
{
  if (vector == NULL)
    error(-1, 0, "delete_ptr_vector(): got NULL ptr");

  if (vector->ptr != NULL)
    free(vector->ptr);
  free(vector);
}

void
ptr_vector_push (PtrVector* vector, void* item)
{
  if (vector->vector_size == vector->vector_max_size)
    vector->ptr = alloc(vector->ptr, sizeof(void*) * ++vector->vector_max_size);

  vector->ptr[vector->vector_size++] = item;
}

void*
ptr_vector_pop (PtrVector* vector)
{
  if (vector->vector_size == 0)
    return NULL;

  return vector->ptr[--vector->vector_size];
}
