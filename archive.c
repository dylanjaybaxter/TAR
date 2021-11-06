/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include "archive.h"

#define BLOCK_SIZE 512

void createArchive(char* dest, char** paths, int pathCount, int option){
    /*Define Variables*/
    int i;
    int fd;

    /*Open New TAR File in destination*/
    if(!(fd = open(dest, O_CREAT|O_TRUNC, 0666))){
        perror("Recursive Stat");
        exit(EXIT_FAILURE);
    }

    /*For each path specified*/
    for(i=0;i<pathCount;i++){
        /*Recursively write each into file*/
        writeRecur(fd, paths[i]);
    }

}

void writeRecur(int fd, char* path){
    /*Open current path*/
    struct stat sb;
    DIR* d;
    struct dirent* e;
    char* newpath = (char*)malloc(256);
    newpath[0] = '\0';
    int flen = 0;
    int plen = 0;

    /*Stat the current target*/
    if(lstat(path, &sb) == -1){
        perror("Recursive Stat");
        exit(EXIT_FAILURE);
    }

    /*If path is a file, write contents to TAR*/
    if(S_ISREG(sb.st_mode)){
        /*Write Header*/
        writeheader(fd, path);

        /*Write Body*/
        writebody(fd, path);

    }

    /*If the items is a symlink*/
    if(S_ISLNK(sb.st_mode)){
        /*Write Header*/
        writeheader(fd, path);

        /*Write Body*/
        writebody(fd, path);

    }

    /*If path is a directory, write contents to file
    Then enter Recursively*/
    if(S_ISDIR(sb.st_mode)){
        /*Write Header*/
        writeheader(fd, path);

        /*Write Body*/
        writebody(fd, path);

        /*Open directory*/
        if((d = opendir(path)) == NULL){
            perror("Recursive Stat");
            exit(EXIT_FAILURE);
        }

        /*For every entry in the directory*/
        while((e = readdir(d))){
            /*Check that new length does not exceed max*/
            flen = strlen(e->d_name);
            plen = strlen(path);
            if((flen+plen) >= PATH_MAX){
                /*Copy the string to the newpath buffer*/
                strcpy(newpath, path);

                /*Add the name to the path*/
                newpath = strncat(newpath, "/" , 2);
                newpath = strncat(newpath, e->d_name, flen+1);

                /*Explore the entry*/
                writeRecur(fd, newpath);
            }
        }
        /*Close directory*/
        if((closedir(d)) == -1){
            perror("Close Dir");
            exit(EXIT_FAILURE);
        }
    }
}

void writebody(int fdout, char* path){
    /*Define variables*/
    char writeBuf[BLOCK_SIZE];
    char readBuf[BLOCK_SIZE];
    int fdin;
    int readSize;

    /*Open read file*/
    if(!(fdin = open(path, O_APPEND, 0666))){
        perror("Body Open");
        exit(EXIT_FAILURE);
    }

    /*While 512 bye blocks exist in read file*/
    while((readSize = read(fdin, readBuf, BLOCK_SIZE))){
        write(fdout, readBuf, BLOCK_SIZE);
    }

    /*Close read file*/
    if((close(fdin) == -1)){
        perror("Body Close");
        exit(EXIT_FAILURE);
    }

}