/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark05)
File: arch_helper.h
Description: This file contains prototypes for functions that create
tar archives
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<getopt.h>
#include<unistd.h>
#include<sys/stat.h>
#include <arpa/inet.h>

char *octalConvert(unsigned int n, char *octal, int len);

int isTAR(char* path, unsigned int options);

int insert_special_int(char * where, size_t size, int32_t val);

void fillArray(char* buff, char fill, int len);