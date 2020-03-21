#ifndef FILE_H
#define FILE_H

#include <stddef.h>

int readc(void);
int unreadc(int c);
int peekc(void);

void writec(const char c);
void writestr(const char* str);
void writestrn(const char* str, size_t size);

void open_file(const char* file_name);
const char* get_file_name(void);

#endif
