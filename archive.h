/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/

struct tarHeader{
    char name[100];
    
};

void createArchive(char* path);

void writeheader(int fd, char* path);

void writebody(int fd, char* path);

void extractArchive(char* path);

void createFile(int tarfd);

