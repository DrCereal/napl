/*  napl - A compiler for the napl programming language
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

#include "file.h"
#include "generate.h"
#include "lexer.h"
#include "parser.h"

int
main (int argc, char** argv)
{
  initialize_lexer();
  initialize_generate();

  open_file_read("test.napl"); 
  open_file_write();

  //parse
  parse();

  write_emission();

  return 0;
}
