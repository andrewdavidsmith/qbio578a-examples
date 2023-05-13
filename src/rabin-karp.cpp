/* rabin-karp: A C++ implementation of (most of) the Rabin-Karp
 *             algorithm that reads the text from a FASTA file. The
 *             parts not included here involve the selection of the
 *             prime number.
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <stdexcept>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::runtime_error;

unsigned char dna_encoding[] = {
/*first*/                                              /*last*/
/*  0*/ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 15*/
/* 16*/ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 31*/
/* 32*/ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 47*/
/* 48*/ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 63*/
/* 64*/ 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, /* 79 (upper) */
/* 80*/ 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /* 95 (upper) */
/* 96*/ 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, /*111 (lower) */
/*112*/ 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, /*127 (lower) */
};
//         ^     ^  ^        ^ (look up from the letters to see the 'enc')
//         A     C           G
//                  T

/* look up in the table above */
static unsigned char
encode_base(const char base) {
  return dna_encoding[base];
}

/* The function below is used to provide an example of how integer
   powers can be computed. The standard functions in C++ require a
   floating point argument, because the return value of integer powers
   need not be an integer, i.e. when the exponent is negative. This
   function works for non-negative integers.
 */
static size_t
nonneg_integer_power(size_t x, size_t n) {
  if (n == 0) return 1;
  size_t y = 1;
  while (n > 1) {
    if (n & 1ul) { // if "n" is odd
      y *= x;
      x *= x;
      n = (n - 1)/2;
    }
    else { // if "n" is even
      x *= x;
      n = (n >> 1);
    }
  }
  return x*y;
}


/* Function to do subtraction modulo q (which should keep the numbers
   positive). I'm not sure if there is a better way to do this, which
   might allow numbers to take negative values for "t" */
inline static size_t
subtract_mod(const size_t a, const size_t b, const size_t q) {
  // if you don't know C or C++, the code below uses what is known as
  // the "ternary operator"
  return (b > a) ? (q - b + a) % q : (a - b) % q;
}


// This is where the Rabin-Karp happens
static size_t
Rabin_Karp(const string &T, const string &P,
           const size_t d, const size_t q, vector<size_t> &matches) {

  const size_t n = P.size();
  const size_t m = T.size();

  const long int h = nonneg_integer_power(d, n-1) % q;

  // compute p and initialize t = t_0
  size_t p = 0;
  size_t t = 0;
  for (size_t i = 0; i < n; ++i) {
    p = (d*p + P[i]) % q;
    t = (d*t + T[i]) % q;
  }

  size_t hit_counter = 0; // counter for hits; only used for analysis

  for (size_t s = 0; s < m - n + 1; ++s) {
    if (p == t) { // filter
      ++hit_counter;
      // below, verify using the built-in C++ "equal" function for any
      // sequence (could be a string or vector, etc.)
      if (std::equal(begin(P), end(P), begin(T) + s))
        matches.push_back(s); // append the match
    }
    if (s < m - n) // shift and update
      t = (d*subtract_mod(t, (T[s]*h) % q, q) % q + T[s+n]) % q;
  }
  return hit_counter;
}


// This function just removes the sequence (e.g. chromosome) names and
// newlines from a string loaded from a FASTA file. It is not
// optimized for speed, but it should be pretty clear.
static void
remove_names_newlines(string &T) {
  bool outside_name = true;
  size_t j = 0;
  const size_t n = T.size();
  for (size_t i = 0; i < n; ++i) {
    const char c = T[i];
    if (outside_name) {
      if (c == '>')
        outside_name = false;
      else if (c != '\n') {
        T[j++] = c;
      }
    }
    else outside_name = (c == '\n');
  }
  // resize but keep capacity
  T.resize(j);
}


// The "get_filesize" below uses some pretty specific C++ code for
// "streams" and if you don't understand it, that's fine.
static size_t
get_filesize(const string &filename) {
  std::ifstream in(filename);
  if (!in)
    throw runtime_error("problem with file: " + filename);
  const std::streampos begin_pos = in.tellg();
  in.seekg(0, std::ios_base::end);
  return in.tellg() - begin_pos;
}


static void
read_fasta_as_one_sequence(const string &fasta_filename, string &T) {
  T.clear(); // start with empty string

  const size_t filesize = get_filesize(fasta_filename);

  // using "C" functions to read in the input because on my Mac there
  // is a problem with reading large files using a single "read"
  // function call when using C++ streams. This problem might be fixed
  // in some compilers, but I've seen it in multiple.
  FILE *in = fopen(fasta_filename.c_str(), "rb");
  if (!in)
    throw runtime_error("problem with file: " + fasta_filename);

  T.resize(filesize); // change *size*, not capacity of T here

  if (fread((char*)&T[0], 1, filesize, in) != filesize)
    throw runtime_error("problem with file: " + fasta_filename);

  if (fclose(in) != 0)
    throw runtime_error("problem with file: " + fasta_filename);

  // remove the sequence names from the FASTA format string, along
  // with the newline characters, what remains should be just DNA
  // bases (maybe with a few random IUPAC degenerate nucleotides)
  remove_names_newlines(T);
}

int
main(int argc, const char * const argv[]) {

  static const size_t d = 5; // using an alphabet size of 5 for the
                             // 'N' in the genome
  static const size_t q = 2557710269ul; // the "ul" means "unsigned long"
  // some other prime numbers (possibilities for "q"):
  // 2557710269ul
  // 573292817ul
  // 3209ul

  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " <pattern> <FASTA-file>" << endl;
    return EXIT_FAILURE;
  }

  const string filename(argv[2]);

  string P(argv[1]);
  string T;
  read_fasta_as_one_sequence(filename, T);

  // make sure pattern not bigger than text
  assert(P.size() <= T.size());

  // convert the pattern and text into their numerical values
  for (size_t i = 0; i < P.size(); ++i)
    P[i] = encode_base(P[i]);

  for (size_t i = 0; i < T.size(); ++i)
    T[i] = encode_base(T[i]);

  // run the actual algorithm
  vector<size_t> matches;
  const size_t hit_counter = Rabin_Karp(T, P, d, q, matches);

  // output the number of matches
  cout << "match count:\t" << matches.size() << endl
       << "hits:\t" << hit_counter << endl
       << "hit rate:\t"
       << static_cast<double>(hit_counter)/T.size() << endl;

  return EXIT_SUCCESS;
}
