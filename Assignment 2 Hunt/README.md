Hunt
=======

A program to recursively traverse directories and find 'matching files' given a specified relative directory.

## Description ##

Given a specific file and a starting path, `hunt` recursively traverses the filesystem, looking for a 'matching' file. After completing a search, any matched files are printed to the output with thir details.

A matching file is considered as a file with equivalent number of bits (file size) as well as the contents within the file (characters data).

After a match has been found further details are categorized as following:

* `hardlink`: The inode of the file as well as device number are identical to the original file, meaning that the file is a direct clone of the original.

* `duplicate`: The inode and/or device number are not identical to the original file, meaning that the file is identical to the target file, but not a clone. If it is a duplicate, the number of links to that file's inode are also specified.

* `symlink`: The directory leads to a file that either resolves to the original file or a hardlink (we do not differentiate the two in this case), or a duplicate.

After the analyzing has been done, the files' permissions are also looked into in order to determine whether an 'other' user could gain access to the directories and files given that they have the directory path to it. Symlinks are not analyzed at this point.

## Compilation ##

To compile, type the following command:

`# gcc -o hunt hunt.c`

## Usage ##

Usage syntax for `hunt` is given by the following:

	./hunt [targetFile] [targetDirectory]
