/* kmp_fasta: A C++ implementation of the Knuth-Morris-Pratt algorithm
 * that takes the text from a FASTA format file and concatenates all
 * sequences in that file (including the name of the sequence).
 *
 * Copyright (C) 2024 Andrew D. Smith
 *
 * Author: Andrew D. Smith
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

// The language used here is roughly C++11. The file IO is done using
// the C functions and not C++ because on Macos, for a long time (I
// haven't checked in the past year) some part of the C++ libaries use
// a signed 32-bit integer function for some buffer, which means that
// reading an entire file of the size of the human genome will simply
// not work. I never bothered to track the bug, since swapping to
// `FILE*` for any single large read is easy.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

// these are to find the input file size and are available in unix/linux/macos
#include <unistd.h>
#include <sys/stat.h>

using std::vector;
using std::string;

using std::begin;
using std::end;

static void
read_fasta_file_single_sequence(const string &filename, vector<char> &T) {

  // In C++17 this can be replaced via std::filesystem
  struct stat st;
  stat(filename.c_str(), &st);
  const size_t filesize = st.st_size;

  T.resize(filesize);

  FILE *in = fopen(filename.c_str(), "rb");
  const size_t r = fread(&T[0], 1, filesize, in);
  if (r != filesize) {
    std::cerr << "failed to read file" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  fclose(in);

  // remove all the newline characters
  const auto x = std::remove(begin(T), end(T), '\n');
  T.resize(std::distance(begin(T), x));
  std::transform(begin(T), end(T), begin(T),
                 [](char x) {return std::toupper(x);});
  // note that if we assume the input FASTA file is the human genome,
  // then this code would remove newlines, but would leave the names
  // of each chromosome intact, and it could cause false-positives for
  // some very short patterns (e.g., if pattern is just "c" since
  // every chrom name has a "c").
}


/*
 * This function computes the "prefix" function for the pattern "P"
 * which is the same as the sp (note: not sp') values in Gusfield's
 * book.
 */
static void
compute_prefix_function(const string &P, vector<size_t> &sp) {
  const size_t n = P.length();
  sp.resize(n, 0);

  size_t k = 0;
  for (size_t i = 1; i < n; ++i) {

    while (k > 0 && P[k] != P[i])
      k = sp[k - 1];

    if (P[k] == P[i]) ++k;

    sp[i] = k;
  }
}


static void
Knuth_Morris_Pratt(const vector<char> &T, const string &P,
                   const vector<size_t> &sp,
                   vector<size_t> &matches) {

  const size_t n = P.size();
  const size_t m = T.size();

  size_t j = 0;
  for (size_t i = 0; i < m; ++i) {

    // look for the longest prefix of P that is the same as a suffix
    // of P[1..j - 1] AND has a different next character
    while (j > 0 && P[j] != T[i])
      j = sp[j - 1];

    // check if the character matches
    if (P[j] == T[i]) ++j;

    // if we have already successfully compared all positions in P,
    // then we have found a match
    if (j == n) {
      matches.push_back(i - n + 1);
      j = sp[j - 1]; // shift by the length of the longest suffix of P
                     // that matches a prefix of P
    }
  }
}


int
main(int argc, const char * const argv[]) {

  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " <pattern> <fasta-file>" << std::endl;
    return EXIT_FAILURE;
  }

  const string P(argv[1]);

  vector<char> T;
  read_fasta_file_single_sequence(argv[2], T);

  vector<size_t> sp;
  compute_prefix_function(P, sp);

  vector<size_t> matches;
  Knuth_Morris_Pratt(T, P, sp, matches);

  std::cout << matches.size() << std::endl;
}
