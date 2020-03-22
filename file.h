#ifndef FILE_H
#define FILE_H

#include <stddef.h>

#include "vector.h"

int readc(void);
int unreadc(int c);
int peekc(void);

void write_buffer (Buffer* buffer);

void open_file_read(const char* file_name);
void open_file_write (void);
const char* get_file_name(void);

#endif
