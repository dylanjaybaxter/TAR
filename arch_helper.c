/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark05)
File: arch_helper.c
Description: This file contains functions used to create
tar archives
*/
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "arch_helper.h"
#include "extr_helper.h"
#include "extract.h"

#define MAXPATH 512

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

int isTAR(char* path, unsigned int options){
    int fd;
    char buffer[MAXPATH];
    struct Header* head;
    int numblocks = 0;
    int endblock = 0;
    int readsize = 0;
    int checkVal = 0;

    if((fd = open(path, O_RDONLY, 0666)) == -1){
        perror("Open isTAR");
        exit(EXIT_FAILURE);
    }

    while((readsize = read(fd, buffer, MAXPATH)) > 0){
        if (numblocks == 0){
            head = (struct Header *)(buffer);
            /*Check for null blocks and invalid headers*/
            checkVal = checksum(head);
            if(checkVal == -1){
                return 0;
            }
            if(checkVal == 0){
                endblock++;
                if(endblock < 1){
                    return 1;
                }
            }
            else{
                if(options & STRICT){
                    if(!((head->version[0] == '0')
                    && (head->version[1] == '0'))){
                        return 0;
                    }
                    if(!strcmp(head->magic,"ustar")){
                        return 0;
                    }
                }else{
                    if((head->magic[0] != 'u') ||
                    (head->magic[1] != 's') ||
                    (head->magic[2] != 't') ||
                    (head->magic[3] != 'a') ||
                    (head->magic[4] != 'r')){
                        return 0;
                    }
                }

                int size = unoctal(head->size);
                /* If there is a remainder we need and entire 512 byte block
                */
                if ((size % 512) == 0){
                    numblocks = size / MAXPATH;
                }
                else{
                    numblocks = (size / MAXPATH) + 1;
                }
            }
        }
        else{
            numblocks--;
        }
    }
    if(-1 == close(fd)){
        perror("Close In isTAR");
        exit(EXIT_FAILURE);
    }
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
