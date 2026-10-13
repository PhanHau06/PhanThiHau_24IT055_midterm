# Midterm Project - Implement ls(1)

**Student:** Phan Thi Hau  
**Student ID:** 24IT055  
**Operating System:** NetBSD 10.1  
**Programming Language:** C  
**Compiler:** GCC 10.5.0  

## 1. Project Description

This project implements a simplified version of the UNIX ls(1) command in C, following the provided NetBSD ls(1) manual.

The program uses UNIX filesystem functions such as opendir(), readdir(), lstat(), and readlink() to retrieve and display file information.

## 2. Project Structure

- main.c: Main program and file/directory handling.
- options.c / options.h: Command-line option parsing using getopt().
- listing.c / listing.h: Directory traversal and recursive listing.
- sort.c / sort.h: File sorting by name, size, and time.
- display.c / display.h: File information and long-format display.
- Makefile: Project compilation.
- .gitignore: Excludes binaries, object files, and backups.

## 3. Implemented Features

The current implementation includes:

- Listing directory contents.
- Displaying hidden files with -a and -A.
- Long-format listing with -l.
- Numeric user and group IDs with -n.
- Displaying inode numbers with -i.
- Displaying directory information with -d.
- Recursive directory traversal with -R.
- Sorting by name, size, or modification time.
- Reversing sorting order with -r.
- Disabling sorting with -f.
- File type indicators with -F.
- Human-readable file sizes with -h.
- Displaying allocated blocks with -s and -k.
- Displaying symbolic link targets in long format.
- Basic error handling for invalid paths.

The -c and -u options are used when selecting timestamps for sorting or display.

Some option interactions and edge cases still require additional testing.

## 4. Compilation

Compile the project using:

    make

Clean compiled files using:

    make clean

The executable is named myls.

## 5. Usage

General syntax:

    ./myls [options] [file ...]

Examples:

    ./myls
    ./myls -a
    ./myls -l
    ./myls -la
    ./myls -lh
    ./myls -F
    ./myls -s
    ./myls -sk
    ./myls -R test_ls
    ./myls -ld .
    ./myls -l options.c

## 6. Testing

The program was compiled and tested on NetBSD 10.1.

Tests included:
- Basic directory listing.
- Hidden files.
- Long-format output.
- Sorting and recursive traversal.
- Symbolic links.
- Block counts and human-readable sizes.
- Regular file and directory operands.

Selected results were compared with the native NetBSD ls command.

## 7. Known Limitations

- Output column alignment is not identical to native ls.
- Full compatibility with BLOCKSIZE is not yet verified.
- The -q and -w options require further implementation/testing.
- Multiple operand ordering and special filesystem cases require further testing.
- Interactions among certain options may differ from native NetBSD ls.

## 8. GitHub Repository

https://github.com/PhanHau06/PhanThiHau_24IT055_midterm
