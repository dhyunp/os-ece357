Minicat
=======

An implementation of the Unix `cat(1)` utility.

## Description ##

The objective for this assignment was to create a "clone" of the UNIX utility **`cat`** called **`copycat`** that provided complete error handling and reporting.  The given constraints were that only standard UNIX system calls (`open()`, `read()`, `write()`, and `close()`) could be used in the manipulation of the user-specified output and input files, along with any data transfer associated with that file manipulation.
## Compilation ##

To compile, type the following command:

# gcc -o minicat minicat.c

## Usage ##

Usage syntax for `copycat` is given by the following:

	minicat [-b ###] [-o outfile] infile1 [...infile2...]
	minicat [-b ###] [-o outfile]
