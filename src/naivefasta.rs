/* naivefasta: An implementation in Rust of the naive algorithm for
 * exact string matching, assuming the text is given as a FASTA format
 * file, and all sequences are concatenated (e.g. chromosomes are put
 * together).
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

// This is not a good way to write any program in Rust, for a variety
// of reasons, but this code uses the language. You wouldn't need to
// use 'cargo' for this, just build it like:
//
// $ rustc naivefasta.rs
//

// This code is not intended to be optimized for speed, or clear for
// readability, but instead to allow a comparison of Rust with the C
// or C++ versions of the same thing.


use std::process;


/// Reads the sequence from a FASTA file as one long sequence of bytes
/// and don't worry about the "name" line of the file but remove all
/// newline chars.
fn read_fasta(
    filename: &String
) -> Result<Vec<u8>, std::io::Error> {

    let mut data = std::fs::read(filename)?;

    let n = data.len();
    let mut j = 0;
    for i in 0..n {
        if data[i] != b'\n' {
            data[j] = data[i];
            j += 1;
        }
    }
    data.resize(j, 0);
    data.shrink_to_fit(); // shrink_to_fit is better in Rust than C++!
    Ok(data)
}


/// Do the naive matching and keep a vector of matching positions
fn naive_match(
    t: &Vec<u8>,
    p: &Vec<u8>,
) -> Vec<usize> {

    let m = t.len();
    let n = p.len();

    // matches as a usize in case the sequence is big, like a genome
    let mut matches: Vec<usize> = vec![];

    // very naive implementation; not what rust users would do
    for i in 0..(m - n + 1) {
        let mut j = 0;
        while j < n && t[i + j] == p[j] {
            j += 1;
        }
        if j == n {
            matches.push(i);
        }
    }
    matches
}


fn main() {

    let args: Vec<String> = std::env::args().collect();
    // no fancy argument parsing for this cli tool.
    if args.len() < 3 {
        eprintln!("input must be: naive_fasta <pattern> <fasta-file>");
        std::process::exit(1);
    }

    let t = read_fasta(&args[2]).unwrap_or_else(|err| {
        eprintln!("{err} : {}", args[2]);
        process::exit(1);
    });
    let p: Vec<u8> = args[1].as_bytes().to_vec();

    let matches = naive_match(&t, &p);

    println!("{}", matches.len());

    std::process::exit(0);
}
