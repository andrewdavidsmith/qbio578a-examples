/* fasta_file: code in C to read a FASTA format file with arbitrary
 * line widths, number of sequences, and sequence lengths.
 *
 * Copyright (C) 2024 Andrew D. Smith
 *
 * Authors: Andrew D. Smith
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef FASTA_FILE_H
#define FASTA_FILE_H

#include <stdlib.h>

int
read_fasta_file(const char *filename, char ***names_out, char ***seqs_out);

void
free_fasta_data(const size_t n_seqs, char **seq_names, char **seqs);

#endif
