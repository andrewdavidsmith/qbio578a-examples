/* aho_corasick: An implementation in C of the Aho-Corasick algorithm
 * for keyword matching.
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

/*
 * This code should compile by doing:
 *
 * $ cc -o aho_corasick aho_corasick.c keyword_tree.c dynamic_array.c fasta_file.c
 *
 * and it should work with any C compiler with c99 and the POSIX function getline.
 */

#include "fasta_file.h"
#include "keyword_tree.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(const int argc, const char **argv) {

  if (argc < 2) {
    fprintf(stderr, "aho_corasic <patterns-fasta> <texts-fasta>\n");
    return EXIT_FAILURE;
  }

  char **pattern_names = NULL;
  char **patterns = NULL;
  const size_t n_patterns = read_fasta_file(argv[1], &pattern_names, &patterns);

  // for (int i = 0; i < n_patterns; ++i)
  //   printf(">%s\n%s\n", pattern_names[i], patterns[i]);

  char **text_names = NULL;
  char **texts = NULL;
  const size_t n_texts = read_fasta_file(argv[2], &text_names, &texts);

  // const size_t text_length = strlen(texts[0]);
  // printf("n_texts=%d\ntext_length=%d\n", n_texts, text_length);

  kw_tree* the_tree = kw_tree_init();

  for (size_t i = 0; i < n_patterns; ++i)
    kw_tree_insert(the_tree, patterns[i], i + 1);

  kw_tree_set_links(the_tree);

  dynamic_array *matches = kw_tree_search(the_tree, texts[0]);

  kw_tree_free(the_tree);

  free_fasta_data(n_patterns, pattern_names, patterns);
  free_fasta_data(n_texts, text_names, texts);

  /* for (int i = 0; i < da_size(matches); ++i) */
  /*   printf("matches[i]=%d\n", da_element_at(matches, i)); */
  printf("%d\n", da_size(matches));

  da_free(matches);

  return 0;
}
