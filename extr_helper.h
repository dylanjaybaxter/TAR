/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark05)
File: extr_helper.h
Description: This file contains prototypes for extr_helper.h
*/
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

uint32_t extract_special_int(char *where, int len);

double pow2(double base, double power);

int unoctal(char *octal);

void ensureDir(char* path);

int checkpre(char* pre, char* str);
