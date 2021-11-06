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
};

void init_Header(struct Header *head);

struct Header *create_header(char *fileName, char option);

void createArchive(char* dest, char** path, int pathCount);

void writeRecur(int fd, char* path);

void writeheader(int fd, char* path);

void writebody(int fd, char* path);

void extractArchive(char* dest, char* path);

void printTAR(int fd, char* path);

void printFile(int fd, char* path);
