#ifndef GENERATE_H
#define GENERATE_H

#include <stddef.h>

void initialize_generate(void);

void Emit_Header(void);

void Emit_Extern(char* name, size_t size);
void Emit_Function_Header(char* name, size_t size);
void Emit_Function_Return(int value);
void Emit_Function_Call(char* name, size_t size, size_t var_count);
void Emit_Variable_Number(int value);
void Emit_Variable_String(char* name, size_t size);

void Emit_Variable_Init(const char* name, int value);

void write_emission(void);

#endif
