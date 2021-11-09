/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<getopt.h>
#include<unistd.h>
#include<sys/stat.h>
#include<grp.h>
#include<pwd.h>
#include"arch_helper.h"

#define BLOCK_SIZE 512
#define MAX_PATH_SIZE 256
#define DEBUG 0
#define CREATE 0x01
#define PRINT 0x02
#define EXTRACT 0x04
#define VERBOSE 0x08
#define FILENAME 0x10
#define STRICT 0x20
#define ALLFLAG 0x40

struct __attribute__((__packed__)) Header{
    /* Header structure
     * Everything is chars because they are either names or strings
     * that portray octal number */
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
};

struct Header* init_Header();

struct Header* create_header(char *fileName, char option);

void createArchive(char* dest, char** paths, int pathCount, int options);

void writeRecur(int fd, char* path, int options);

void writeheader(int fd, char* path, int options);

void writebody(int fd, char* path);

void writePad(int fd);
