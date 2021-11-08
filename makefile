CC = gcc
CFLAGS = -Wall -g
LD = gcc
LDFLAGS = -g

all: mytar archive.o arch_helper.o

mytar: archive.o arch_helper.o mytar.o
	$(LD) $(LDFLAGS) -o mytar mytar.o archive.o arch_helper.o

mytar.o: archive.c arch_helper.c mytar.c
	$(CC) $(CFLAGS) -c -o mytar.o mytar.c
archive.o: archive.c
	$(CC) $(CFLAGS) -c -o archive.o archive.c
arch_helper.o: arch_helper.c
	$(CC) $(CFLAGS) -c -o arch_helper.o arch_helper.c

clean: mytar
	rm archive.o arch_helper.o
run: mytar
	rm archive.o arch_helper.o
	./mytar cf t.tar Fold
debug: mytar
	gdb mytar
valgrind: mytar
	valgrind -s --leak-check=full --show-leak-kinds=all ./mytar cf t.tar Fold