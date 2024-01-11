# qbio578a-examples
I'm using this repo to share some code I've used in the past for teaching.

Code in this repo is written in different languages, using different styles,
and for different purposes: some will be optimized for speed, while others
designed for readability.

I'll probably add source files here gradually.

This the repo currently contains source files in C, C++ and Rust. Each
of these can be pretty fast, and I don't think it's possible to say
that any one of those is faster than the others. Only in certain
situations. The speed of the code is ultimately determined by the
compiler's optimizations. Some styles of coding allow compilers to see
more optimizations, and some languages (e.g., modern C++ and Rust)
emphasize practices that help compilers detect ways to optimize the
code. At the same time, different compilers can generate code with
dramatically different speeds. Unfortunately when we write code that
we optimize for speed, some compiler optimizations can impact the
accuracy of the code. Probably not for us, in QBIO 578a, however.

## Compiling the code

### The "naive" algorithm for exact string matching

First example: `naive.c` is the naive algorithm for extract string
matching, written in "old school" C, which might look slightly funny
compared to more modern C. There are reasons why someone might still
write C that looks like this.

Here is how to compile the `naive` binary and run it:
```
cc -o naive_c_version naive.c
```

Most of you will be using `gcc` as your C compiler, and on most
systems anyone uses in the sciences, the `cc` compiler is actually
just a symbolic link to a GCC compiler. This is true of linux and
macos. Among free compilers, the LLVM compilers (which provides clang
and clang++) are maintained by Google and are as good as GCC --
sometimes better.

To run `naive` do it like this, with a pattern preceding a text on the
command line:
```
./naive_c_version AGT GTTATAGTTGTAGGGATGAAGTCTCGTTGTTGTTACTTATCTCAAGTT
```

Moving to `naive.cpp` which is identical to `naive.c` except that it
is written in C++. The use of C++ here is also not modern. You should
be able to see the correspondence between the parts of the source
code. Compile it like this:
```
c++ -o naive_cpp_version naive.cpp
```
and run it similarly to the above:
```
./naive_cpp_version AGT GTTATAGTTGTAGGGATGAAGTCTCGTTGTTGTTACTTATCTCAAGTT
```

In both cases, the names of the binaries are arbitrary so you can name
things whatever you want with the `-o`.

As you know from class, the naive algorithm exists here just for a
baseline to compare with. But these examples should also be helpful if
you have never seen a memory-managed language before, because the
`grow_matches_array` function in `naive.c` demonstrates how simple
dynamic arrays work. This behavior is the basis for any dynamic arrays
you might use in Python or R.

### The Z algorithm

This algorithm is much faster in general. In fact we know the Z
algorithm gives us a linear time approach while the niave algorithm is
essentially quadratic time if we assume the pattern and text have size
that's within a constant factor of each other (e.g., the pattern might
be 1/2 the size of the text).

To compile the implementation of the Z algorithm's proprocessing:
```
c++ -o z_algorithm z_algorithm.cpp
```
And to run the code:
```
./z_algorithm GTTATAGTTGTAGGGATGAAGTCTCGTTGTTGTTACTTATCTCAAGTT
```

The output will be a "trace" of the algorithm, indicating for each
iteration, how the variables change, and then finally producing the Z
values. By itself this might not seem like it can solve the exact
string matching problem, but it can if we manipulate the input a bit.

So far none of these implementations are setup to allow us to compare
their performance.
