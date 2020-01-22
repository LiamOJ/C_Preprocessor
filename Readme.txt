Author: LiamOJ
Date of Last Change: 01/03/2019
Program name: preprocess.c

Program Purpose: 
To imitate the first few steps of a C preprocessor and parse a .c file to remove
comments, count non-empty lines, count comments, replace header file calls with content and replace constants with values.
The output file will retain the input file name, but output with a .o extension. 

Program use: 
To compile - 
Using the Microsoft C/C++ compiler use 'cl preproces.c'
There are no other libraries, c files or header files needed at compile time. 

To use, from the command line - 
To indicate the input file use the following command '-i "filename"'
To indicate to keep comments in the output file use the following command '-c' at any point in the command line instruction

To use, via makefile commands -
maths - processes math_functions.c, no comments
mathsC - as above, with comments
strings - processes string_function.c, no comments
stringsC - as above, with comments.
clean - deletes .obj and .exe files. 


Tool chain:
Microsoft C/C++ Compiler
