CC = gcc

mycp:main.o SimpleCopyFile.o ArguMethod.o
	$(CC) -g -o mycp SimpleCopyFile.o ArguMethod.o main.o 
main.o:main.c CopyFile.h
	$(CC) -g -c main.c 
SimpleCopyFile.o:SimpleCopyFile.c
	$(CC) -g -c SimpleCopyFile.c 
ArguMethod.o:ArguMethod.c CopyFile.h
	$(CC) -g -c ArguMethod.c  

