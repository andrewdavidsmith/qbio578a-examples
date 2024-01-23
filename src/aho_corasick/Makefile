# Makefile for the aho_corasick algorithm implementation in C
#
# Copyright (C) 2024 Andrew D. Smith
#
# Authors: Andrew D. Smith
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# ADS: Always try to compile cleanly without warnings.  The "gnu99" is
# for the "getline" function which was originally a GNU extension and
# subsequently a POSIX function in 2008.
CFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Werror -Wfatal-errors
CC = gcc

all: aho_corasick

aho_corasick: aho_corasick.c keyword_tree.c dynamic_array.c fasta_file.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f aho_corasick
