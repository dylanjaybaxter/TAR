CC = gcc
CFLAGS = -Wall -g -ansi
LD = gcc
LDFLAGS = -g

all: mytar archive.o arch_helper.o

mytar: archive.o arch_helper.o mytar.o
	$(LD) $(LDFLAGS) -o mytar mytar.o archive.o arch_helper.o

mytar.o: archive.c arch_helper.c
	$(CC) $(CFLAGS) -c -o mytar.o mytar.c
archive.o: archive.c
	$(CC) $(CFLAGS) -c -o archive.o archive.c
arch_helper.o: arch_helper.c
	$(CC) $(CFLAGS) -c -o arch_helper.o arch_helper.c

clean: mytar
	rm archive.o arch_helper.o
debug: hencode hdecode
	gdb mytar
valgrind: mytar
	valgrind -s --leak-check=full --show-leak-kinds=all ./mytar st.txt