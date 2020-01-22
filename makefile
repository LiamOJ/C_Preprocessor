clean:
	del *.obj
	del *.exe

strings:
	cl preprocess.c 
	preprocess -i string_functions.c

stringsC:
	cl preprocess.c
	preprocess -i string_functions.c -c

maths:
	cl preprocess.c
	preprocess -i math_functions.c

mathsC:
	cl preprocess.c
	preprocess -i math_functions.c -c
	