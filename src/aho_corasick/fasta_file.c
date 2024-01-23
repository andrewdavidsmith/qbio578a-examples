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

#include "fasta_file.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef max
#undef max
#endif
#define max(a,b) ((a) > (b) ? (a) : (b))


void
free_fasta_data(const size_t n_seqs, char **seq_names, char **seqs) {
  for (size_t i = 0; i < n_seqs; ++i) {
    free(seq_names[i]);
    free(seqs[i]);
  }
  free(seq_names);
  free(seqs);
}


int
read_fasta_file(const char *filename, char ***names_out, char ***seqs_out) {
  static const size_t BUFFER_SIZE = 1024;
  static const size_t SEQS_CAPACITY_INIT = 8;

  FILE *in = fopen(filename, "r");
  if (in == NULL)
    return -1;

  size_t n_seqs = 0;
  size_t seqs_cap = SEQS_CAPACITY_INIT;

  char **names = calloc(seqs_cap, sizeof(char*));
  char **seqs = calloc(seqs_cap, sizeof(char*));

  size_t seq_len = 0;
  size_t seq_cap = BUFFER_SIZE;  // does not include '\0'

  char *buffer = NULL;
  size_t buffer_cap = 0;
  while (getline(&buffer, &buffer_cap, in) > 0) {
    const size_t line_width = strcspn(buffer, "\n");

    if (buffer[0] == '>') {

      // if we will overflow our capacity, grow the arrays
      if (n_seqs >= seqs_cap) {
        seqs_cap *= 2;
        seqs = realloc(seqs, seqs_cap*sizeof(char*));
        names = realloc(names, seqs_cap*sizeof(char*));
      }

      // allocate space for the sequence name
      names[n_seqs] = calloc(line_width, sizeof(char));

      // skip the '>' character in the buffer when copying the name
      strncpy(names[n_seqs], buffer + 1, line_width - 1);

      // allocate space for the sequence to follow
      seq_cap = BUFFER_SIZE;
      seqs[n_seqs] = calloc(seq_cap + 1, sizeof(char));
      seq_len = 0;

      ++n_seqs;
    }
    else {

      // check if we need more capacity for the sequence
      if (seq_len + line_width > seq_cap) {
        seq_cap = max(2*seq_cap, seq_len + line_width);
        seqs[n_seqs-1] = realloc(seqs[n_seqs-1], (seq_cap + 1)*sizeof(char));
      }

      // copy the current line into the growing sequence
      strncpy(seqs[n_seqs-1] + seq_len, buffer, line_width);
      seq_len += line_width;
      seqs[n_seqs-1][seq_len] = '\0';  // since realloc won't add '\0'
    }
  }

  free(buffer);
  fclose(in);

  *names_out = names;
  *seqs_out = seqs;

  return n_seqs;
}
