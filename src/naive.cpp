/* naive: An implementation in C++ of the naive algorithm for exact
 * matching of a pattern in a text. This does not use the most modern
 * C++, and I don't think anything here is from C++11 or onward.
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
 * $ c++ -o naive naive.cpp
 *
 * and it should work with any C++ compiler from the past 10 years,
 * though possibly there are some things here that won't work in the
 * earliest compilers (e.g., the ifstream constructed from a string).
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::vector;
using std::string;
using std::cout;
using std::endl;


/* This is a pretty slow (for C++), but clear and clean way to read a
   FASTA format file, assuming it has only one sequence that might
   span multiple lines. In general FASTA format can have many
   sequences, and if that were the case, this function would not work.
 */
static void
read_fasta_file_single_sequence(const string &filename, string &T) {

  std::ifstream in(filename);
  if (!in)
    return; // this should be checked as an error

  string line;
  in >> line;
  while (in >> line)
    T += line;
}

int main(int argc, const char * const argv[]) {

  if (argc != 3) {
    std::cerr << "must give a string as input!" << endl;
    return EXIT_FAILURE;
  }

  const string P(argv[1]);
  const string filename(argv[2]);

  // read the text from the specified file
  string T;
  read_fasta_file_single_sequence(filename, T);

  const size_t m = T.length();
  const size_t n = P.length();

  vector<size_t> matches;

  const size_t lim = m - n + 1;
  for (size_t i = 0; i < lim; ++i) {
    size_t j = 0;
    while (j < n && P[j] == T[i + j]) ++j;
    if (j == n)
      matches.push_back(i);
  }

  // for (size_t i = 0; i < matches.size(); ++i)
  //   cout << matches[i] << endl;

  cout << "n_matches=" << matches.size() << endl;

  return EXIT_SUCCESS;
}
