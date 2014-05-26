CC = gcc
Cflags = -g
mycp:main.o SimpleCopyFile.o ArguMethod.o hash.o CopyFile.o 
	$(CC) $(Cflags) -o mycp SimpleCopyFile.o ArguMethod.o CopyFile.o hash.o main.o 
main.o:main.c CopyFile.h
	$(CC) $(Cflags) -c main.c 
SimpleCopyFile.o:SimpleCopyFile.c  CopyFile.h
	$(CC) $(Cflags) -c SimpleCopyFile.c 
ArguMethod.o:ArguMethod.c  CopyFile.h 
	$(CC) $(Cflags) -c ArguMethod.c  
hash.o:hash.c hash.h
	$(CC) $(Cflags) -c hash.c
CopyFile.o:CopyFile.h CopyFile.c
	$(CC) $(Cflags) -c CopyFile.c
clean:
	rm -f *.o mycp
