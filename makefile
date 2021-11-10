CC = gcc
CFLAGS = -pedantic -Wall -g
LD = gcc
LDFLAGS = -g

all: mytar archive.o arch_helper.o

mytar: archive.o arch_helper.o mytar.o extract.o extr_helper.o contents.o
	$(LD) $(LDFLAGS) -o mytar mytar.o archive.o arch_helper.o \
	extract.o extr_helper.o contents.o

mytar.o: archive.c arch_helper.c mytar.c
	$(CC) $(CFLAGS) -c -o mytar.o mytar.c
archive.o: archive.c
	$(CC) $(CFLAGS) -c -o archive.o archive.c
arch_helper.o: arch_helper.c
	$(CC) $(CFLAGS) -c -o arch_helper.o arch_helper.c
extract.o: extract.c extract.h
	$(CC) $(CFLAGS) -c -o extract.o extract.c
extr_helper.o: extr_helper.c extr_helper.h
	$(CC) $(CFLAGS) -c -o extr_helper.o extr_helper.c
contents.o: contents.c contents.h
	$(CC) $(CFLAGS) -c -o contents.o contents.c

clean: mytar
	rm archive.o arch_helper.o extract.o extr_helper.o mytar.o
run: mytar
	rm archive.o arch_helper.o extract.o extr_helper.o mytar.o
	./mytar cf t.tar Fold
	./mytar tvf t.tar
	./mytar xf t.tar Fold/
bin: mytar
	rm archive.o arch_helper.o extract.o extr_helper.o mytar.o
	./mytar cf b.tar test.bin
	./mytar tvf b.tar
	./mytar xf b.tar
debug: mytar
	gdb mytar
valgrind: mytar
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all \
	./mytar cf t.tar Fold
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all \
	./mytar xf t.tar Fold/