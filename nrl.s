/*  nrl.s - NAPL's runtime library.
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

#NAPL's runtime library.

.section .text
.extern main
.globl _start
_start:
  
  call main
 
  mov %eax, %ebx
  mov $1, %eax
  int $0x80

#This function is still a WIP
.globl print
print:
  push %rbp
  mov %rsp, %rbp

  movq %rdi, -16(%rbp)  #pointer
  movq %rsi, -24(%rbp)  #size

  movq $4, %rax
  movq $1, %rbx

  movq -16(%rbp), %rcx
  cmp $0, %rcx
  je print_nl
  movq -24(%rbp), %rdx
  jmp print_ret

print_nl:
  movq $new_line, %rcx
  movq $1, %rdx

print_ret:
  int $0x80
  pop %rbp
  ret

.section .data
new_line:
  .ascii "\n"
