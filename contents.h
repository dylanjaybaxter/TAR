/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark05)
File: contents.h
Description: This file contains prototypes for contents.c
*/
#include "extract.h"

void printContents(char **fileNames, int pathcount,
     char *archive, unsigned int options);

void permissions(mode_t mode, char* perm, struct Header* head);