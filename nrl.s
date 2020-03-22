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

#rdi = pointer to string
.globl print1
print1:
  push %rbp
  movq %rsp, %rbp

  movq %rdi, %rcx
.loop:
  movq $4, %rax
  movq $1, %rbx
  movq $1, %rdx
  cmpb $0, (%rcx)
  je .ret

  pushq %rcx
  int $0x80
  popq %rcx

  addq $1, %rcx
  jmp .loop
.ret:
  pop %rbp
  ret
