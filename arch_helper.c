/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "archive.h"
#include "arch_helper.h"

/*Converts and integer into and ascii string in octal*/
char *octalConvert(unsigned int n, char *octal, int len){
    /* Takes a char pointer of a given size and creates an octal number
     * from the given integer.*/
     /*Fill with zeros*/
     int i=0;
     for(i=0;i<len;i++){
         octal[i] = '0';
     }
     /*Special Case - n=0*/
    if(n == 0){
        /*Null terminate if room*/
        if(len > 1){
            octal[len-1] = '\0';
        }
        return octal;
    }

    /*Generate reversed octal string*/
    int nt = n;
    i=0;
    while (nt != 0){
        octal[i] = 48 + (nt % 8);
        nt = nt / 8;
        i++;
    }

    /*Reverse back string*/
    int j;
    char* temp = (char*)malloc(sizeof(char)*len);
    strncpy(temp, octal, len);
    for (j = len - 2, i = 0; i < len-1; i++, j--){
        octal[i] = temp[j];
    }

    /*Null Terminate if room*/
    octal[len-1] = '\0';
    free(temp);
    return octal;
}

void fillArray(char* buff, char fill, int len){
    int i;
    for(i=0;i<len;i++){
        buff[i] = fill;
    }
}

int isTAR(char* path){
    return 1;
}

int insert_special_int(char * where, size_t size, int32_t val){
    int err = 0;

    if (val < 0 || (size < sizeof(val))){
        err++;
    } else {
        memset(where, 0, size);
        *(int32_t *)(where+size-sizeof(val)) = htonl(val);
        *where |= 0x80;
    }
    return err;
}
