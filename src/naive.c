/* naive: An implementation in C of the naive algorithm for exact
 * matching of a pattern in a text. This is not optimized for speed,
 * but intended to be clear on how to grow an array by reallocating
 * memory. I tried to use old-school C in this example so students
 * would get a sense of what different "eras" of C code might look
 * like.
 *
 * Copyright (C) 2023 Andrew D. Smith
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
 * This code should compile with just this:
 *
 * $ cc -o naive naive.c
 *
 * and it should work with any C compiler.
 */
#include <stdio.h>  // for printing
#include <string.h> // for "strlen"
#include <stdlib.h> // for malloc

int *
grow_matches_array(const int n_matches, int *const matches, int *capacity) {
  int *tmp;
  int i;

  *capacity = (*capacity)*2; // make the capacity twice as much
  // allocate the new space
  tmp = (int *)malloc((*capacity)*sizeof(int));
  // copy the previous results into the new space
  for (i = 0; i < n_matches; i++)
    tmp[i] = matches[i];

  free(matches); // release previously used memory

  return tmp; // this will become the new "matches" when passed back
}

int main(const int argc, char *const argv[]) {

  // Note: Originally the C language required variable declaration
  // here, modern C does not.
  char *P, *T;    // character arrays (pointers) for pattern and text
  int n, m;       // these will be the lengths of pattern and text
  int i, j;       // variables we will need
  int *matches;   // array where we will store matches
  int n_matches;  // counter for the number of matches
  int capacity;   // to keep track of the size of "matches"

  if (argc != 3) {
    printf("input must be: naive <pattern> <text>\n");
    return -1;
  }

  // ADS: note below that assigning P and T does not copy, while
  // determining their length takes linear time within 'strlen'
  P = argv[1];   // let P point to the first arg
  n = strlen(P); // pattern length

  T = argv[2];   // and T will point to the second arg
  m = strlen(T); // length of text

  capacity = 1; // start with at least some space to store matches --
                // you can't double the capacity of it starts at 0...

  // allocate the initial memory for storing match locations
  matches = (int *)malloc(sizeof(int)*capacity);
  n_matches = 0;

  // In something like 'vector' in the C++ stl, the vector itself
  // would have three data elements defined and they would be the same
  // as the (matches, n_matches, capacity). The difference would be
  // that a size_t would be used in C++ for both the capacity and the
  // size (where 'size' is 'n_matches').

  // The naive string matching algorithm
  for (i = 0; i < m - n + 1; i++) {
    j = 0;
    while (j < n && (T[i + j] == P[j]))
      j++;
    if (j == n) {
      matches[n_matches] = i;
      n_matches++;
      // if there is no room left in the matches array, grow it!
      if (n_matches == capacity)
        matches = grow_matches_array(n_matches, matches, &capacity);
    }
  }

  /* for (i = 0; i < n_matches; ++i) */
  /*   printf("%d\n", matches[i]); */
  printf("n_matches=%d\n", n_matches);
  printf("capacity=%d\n", capacity);

  // free the memory we had allocated for storing matches
  free(matches);

  return 0;
}
