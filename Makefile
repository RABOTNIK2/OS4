CC = gcc
CFLAGS = -O2 -Wall

all: impl1.dll impl2.dll prog1.exe prog2.exe

impl1.dll: impl1.c contract.h
	$(CC) $(CFLAGS) -shared -o impl1.dll impl1.c -Wl,--out-implib,libimpl1.a

impl2.dll: impl2.c contract.h
	$(CC) $(CFLAGS) -shared -o impl2.dll impl2.c -Wl,--out-implib,libimpl2.a

prog1.exe: prog1.c contract.h impl1.dll
	$(CC) $(CFLAGS) -o prog1.exe prog1.c -L. -limpl1

prog2.exe: prog2.c contract.h
	$(CC) $(CFLAGS) -o prog2.exe prog2.c

clean:
	del /Q *.dll *.exe *.a 2>NUL