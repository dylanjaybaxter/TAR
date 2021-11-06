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
#include"arch_helper.h"

struct tarHeader{
    char name[100];
    uint8_t mode;


};
    
void createArchive(char* dest, char** path, int pathCount);

void writeRecur(int fd, char* path);

void writeheader(int fd, char* path);

void writebody(int fd, char* path);

void extractArchive(char* dest, char* path);

void createFile(int tarfd);

int isTAR(char* path);

void printTAR(int fd, char* path);

void printFile(int fd, char* path);
