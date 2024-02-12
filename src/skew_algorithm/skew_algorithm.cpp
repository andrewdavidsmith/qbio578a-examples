/* skew_algorithm: A C++ implementation of suffix array construction
 * using the algorithm of Karkkainen & Sanders (2003).
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
  ADS: I assume this is entirely C++11. You should be able to compile it like:

  $ g++ -o skew_algorithm skew_algorithm.cpp

  If your C++ compiler is older, then you might need something like:

  $ g++ -std=c++11 -o skew_algorithm skew_algorithm.cpp
*/


#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <numeric>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::sort;
using std::ios;
using std::partial_sum;


static inline bool
leq_pair(const uint32_t a1, const uint32_t a2,
         const uint32_t b1, const uint32_t b2) {
  return (a1 < b1 || (a1 == b1 && a2 <= b2));
}


static inline bool
leq_triple(const uint32_t a1, const uint32_t a2, const uint32_t a3,
           const uint32_t b1, const uint32_t b2, const uint32_t b3) {
  return (a1 < b1 || (a1 == b1 && leq_pair(a2, a3, b2, b3)));
}


static void
counting_sort(vector<uint32_t> &a, vector<uint32_t> &b,
           vector<uint32_t>::const_iterator r, size_t n, size_t K) {

  vector<uint32_t> c(K + 1, 0);
  const vector<uint32_t>::const_iterator a_beg = cbegin(a);
  const vector<uint32_t>::const_iterator a_lim = a_beg + n;

  vector<uint32_t>::const_iterator a_itr = a_beg;
  for (; a_itr != a_lim; ++a_itr)
    ++c[*(r + *a_itr)];

  for (size_t i = 1; i <= K; i++)
    c[i] += c[i-1];

  while (a_itr != a_beg)
    b[--c[*(r + *a_itr)]] = *(--a_itr);
}


static void
skew(const vector<uint32_t> &s, vector<uint32_t> &SA, const size_t n, const size_t K) {

  const size_t n0 = (n + 2)/3;  // mod0 suffixes
  const size_t n1 = (n + 1)/3;  // mod1 suffixes
  const size_t n2 = (n + 0)/3;  // mod2 suffixes
  const size_t n02 = n0 + n2;   // mod0 and mod2 suffixes (why?)

  // ADS: think about why the "+ 3" is used below
  vector<uint32_t> s12(n02 + 3, 0);

  // ADS: why the iteration limit of n + (n0-n1)?
  for (size_t i = 0, j = 0; i < n + (n0-n1); ++i)
    if (i % 3 != 0)
      s12[j++] = i;

  vector<uint32_t> SA12(n02 + 3, 0);

  // Together these counting sorts below form a radix sort on triples
  counting_sort(s12, SA12, begin(s) + 2, n02, K);
  counting_sort(SA12, s12, begin(s) + 1, n02, K);
  counting_sort(s12, SA12, begin(s) + 0, n02, K);

  const vector<uint32_t>::const_iterator sbeg = cbegin(s);

  // ADS: for the uint32_t below with value -1 it is actually the
  // wrapping to the largest value of a uint32_t
  size_t name = 0;
  uint32_t c0 = -1, c1 = -1, c2 = -1;
  const vector<uint32_t>::const_iterator lim = cbegin(SA12) + n02;
  for (vector<uint32_t>::const_iterator i = cbegin(SA12); i != lim; ++i) {
    const vector<uint32_t>::const_iterator triplet_start = sbeg + *i;
    if (*(triplet_start + 0) != c0 ||
        *(triplet_start + 1) != c1 ||
        *(triplet_start + 2) != c2) {
      name++;
      c0 = *(triplet_start + 0);
      c1 = *(triplet_start + 1);
      c2 = *(triplet_start + 2);
    }
    if (*i % 3 == 1)
      s12[*i/3] = name;
    else
      s12[*i/3 + n0] = name;
  }

  if (name == n02) {
    // here the names are unique, so are the ranks
    for (size_t i = 0; i < n02; ++i)
      SA12[s12[i]-1] = i;
  }
  else {
    // here we must recurse to resolve non-unique ranks
    SA12.clear();
    skew(s12, SA12, n02, name);
    for (size_t i = 0; i < n02; ++i)
      s12[SA12[i]] = i + 1;
  }

  vector<uint32_t> s0(n0);
  for (size_t i = 0, j = 0; i < n02; ++i)
    if (SA12[i] < n0)
      s0[j++] = 3*SA12[i];

  vector<uint32_t> SA0(n0);
  counting_sort(s0, SA0, begin(s), n0, K);
  s0.clear();
  s0.shrink_to_fit();

  SA.resize(n);
  for (size_t p = 0, t = (n0-n1), k = 0; k < n; ++k) {

    const size_t i = SA12[t] < n0 ? SA12[t]*3 + 1 : (SA12[t] - n0)*3 + 2;
    const size_t j = SA0[p];

    const bool compare_pairs = SA12[t] < n0;
    const bool mod12_is_smaller =
      (compare_pairs ?
       leq_pair(s[i], s12[SA12[t] + n0], s[j], s12[j/3]) :
       leq_triple(s[i], s[i+1], s12[SA12[t] + 1 - n0],
                  s[j], s[j+1], s12[j/3 + n0]));

    if (mod12_is_smaller) {
      SA[k] = i;
      ++t;
      if (t == n02)
        for (k++; p < n0; p++, k++)
          SA[k] = SA0[p];
    }
    else {
      SA[k] = j;
      ++p;
      if (p == n0)
        for (++k; t < n02; ++t, ++k)
          SA[k] = (SA12[t] < n0 ?
                   SA[k] = SA12[t]*3 + 1 :
                   (SA12[t] - n0)*3 + 2);
    }
  }
}


// Reads a FASTA format file line-by-line, skipping the "name" lines.
// This function is not designed to read FASTA format files generally.
static vector<uint32_t>
read_fasta_as_numbers(const string &fasta_filename) {
  // see the Rabin-Karp source for more on this encoding
  static constexpr char dna_encoding[] = {
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 1, 5, 2, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 1, 5, 2, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
  }; // note: there is no "0" in this encoding because we need it for
     // our termination symbols and to ensure it is always preceding
     // any other letter in any new alphabet

  std::ifstream in(fasta_filename);
  if (!in)
    throw std::runtime_error("problem with file: " + fasta_filename);

  // get the size of the file the pre-C++17 way: find the size of the
  // file by moving to end, then finding the distance from the
  // beginning
  const std::streampos begin_pos = in.tellg();
  in.seekg(0, std::ios_base::end);
  const size_t filesize = in.tellg() - begin_pos;
  in.seekg(0, std::ios_base::beg); // move back to the beginning

  vector<uint32_t> T;
  T.reserve(filesize); // reserve enough total space

  string line;
  while (getline(in, line))
    if (line[0] != '>') // don't do the "name" lines in FASTA
      // the "transform" puts the current line into the text, while
      // converting the letter to its numerical encoding
      transform(begin(line), end(line), back_inserter(T),
                [](const char c) {
                  // cast to integer type to avoid compiler warning
                  return dna_encoding[static_cast<uint32_t>(c)];
                });

  // we may assume copy elision for any modern C++
  return T;
}


int
main(int argc, const char **argv) {

  try {

    static const size_t initial_alphabet_size = 5;

    if (argc != 3) {
      cout << "usage: " << argv[0] << " <fasta-file> <outfile>" << endl;
      return EXIT_SUCCESS;
    }

    const string filename(argv[1]);
    const string outfile(argv[2]);

    /* opening the output stream in binary mode */
    // ADS: do this *now* in case it fails we won't have spent all the
    // time loading the input or making the suffix array...
    std::ofstream out(outfile, std::ios::out | std::ios::binary);
    if (!out)
      throw std::runtime_error("problem with file: " + outfile);

    // Now load the "text" T (below) as a numerical format right away,
    // since the general recursive skew function needs to
    // accept arbitrary alphabet, which might need to grow larger than
    // "char" would allow -- hence using some larger integer values.

    /* here I'm using a 32-bit unsigned: uint32_t */
    /* this is enough for the human genome (one strand) */

    vector<uint32_t> T = read_fasta_as_numbers(filename);

    // ADS: Adding 3 zeros because every triplet must be complete and
    // a full triplet of 000 is needed in case (n = 1 mod 3) since,
    // for any mod0, we need a mod12 that follows it. This will happen
    // again recursively inside "skew".
    const size_t n = T.size();
    T.push_back(0);
    T.push_back(0);
    T.push_back(0);

    vector<uint32_t> SA;
    skew(T, SA, n, initial_alphabet_size);

    const size_t n_bytes_to_write = SA.size()*sizeof(uint32_t);

    // below the "reinterpret_cast" is required because the file
    // output deals with characters ("char") one byte each, but our
    // data to write is in the form of uint32_t values.

    out.write(reinterpret_cast<const char*>(&SA[0]), n_bytes_to_write);
    out.close();
  }
  catch (std::exception &e) {
    std::cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
