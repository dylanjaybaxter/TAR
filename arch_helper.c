/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/
#include<string.h>
#include "arch_helper.h"


char *octalConvert(unsigned int n, char *octal){
    /* Takes a char pointer of a given size and creates an octal number
     * from the given integer.  */
    int i = 0;
    while (n != 0){
        octal[i] = 48 + (n % 8);
        n = n / 8;
        i++;
    }
    int length = i;
    int j;
    int templen = strlen(octal);
    char *temp = (char*) malloc(templen * sizeof(char));
    strcpy(temp, octal);
    for (j = length - 1, i = 0; i < length; i++, j--){
        octal[i] = temp[j];
    }
    free(temp);
    return octal;
}