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


void init_Header(struct Header *head){
    /* Initializes the header */
    char *smallOct = (char *) malloc(8);
    char *bigOct = (char *) malloc(12);
    head->mode = octalConvert(0, smallOct); /* THESE ARE ALL THE SAME POINTER*/
    head->uid = octalConvert(0, smallOct);
    head->gid = octalConvert(0, smallOct);
    head->size = octalConvert(0, bigOct);
    head->mtime = octalConvert(0, bigOct);
    head->chksum = octalConvert(0, smallOct);
    head->typeflag = '0';
    strcpy(head->name, "0");
    strcpy(head->linkname, "0");
    strcpy(head->magic, "ustar");
    head->version[0] = '0';
    head->version[1] = '0';
    strcpy(head->uname, "0");
    strcpy(head->gname, "0");
    head->devmajor = octalConvert(0, smallOct);
    head->devminor = octalConvert(0, smallOct);
    strcpy(head->prefix, "0");
    free(smallOct);
    free(bigOct);
}



struct Header *create_header(char *fileName, char option){
    /* Takes the given file name and creates a header. It also takes in an
     * option variable to check if the Strict mode has been set */
    struct Header *head = (struct Header *) malloc(sizeof(struct Header));
    char *smallOct = (char *) malloc(8); /* Max Octal Size for multiple vars */
    char *bigOct = (char *) malloc(12); /* Max OCtal Size for larger vars */
    init_Header(head);
    int length = strlen(fileName);
    int i;
    int index;

    struct stat file;
    lstat(fileName, &file);

    head->mode = octalConvert(file.st_mode, smallOct);
    head->uid = octalConvert(file.st_uid, smallOct);
    head->gid = octalConvert(file.st_gid, smallOct);
    head->size = octalConvert(file.st_size, bigOct);
    head->mtime = octalConvert(file.st_mtime, bigOct);

    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(file.st_gid);
    pwd = getpwuid(file.st_uid);
    strcpy(head->uname, pwd->pw_name);
    strcpy(head->gname, grp->gr_name);
    /*NEED TO CHECK THE LENGTH OF EVERYTHING, NOT JUST THE PATH*/
    if (length <= 256 || option == 'S'){
        for (i = length; i > -1; i--){
            if (fileName[i] == '/'){
                index = i + 1;
            }
        }
        char pre[155] = {0};
        if (index - 1 > 0){
            for (i = 0; i < (index = 1); i++){
                pre[i] = fileName[i];
            }
        }
        strcpy(head->name, fileName + index);
        strcpy(head->prefix, pre);
    }
    else{
       perror("File name is too long");
       exit(EXIT_FAILURE);
    }
    free(smallOct);
    free(bigOct);
    return head;
}

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
    int i;

    /*Open read file*/
    if(!(fdin = open(path, O_APPEND, 0666))){
        perror("Body Open");
        exit(EXIT_FAILURE);
    }

    /*While 512 bye blocks exist in read file*/
    while((readSize = read(fdin, readBuf, BLOCK_SIZE))){
        /*If if on last block, pad with 0's*/
        if(readSize < BLOCK_SIZE){
            for(i=readSize;i<(BLOCK_SIZE);i++){
                readBuf[i] = '\0';
            }
        }
        /*Write block*/
        write(fdout, readBuf, BLOCK_SIZE);
    }

    /*Close read file*/
    if((close(fdin) == -1)){
        perror("Body Close");
        exit(EXIT_FAILURE);
    }

}

void writeheader(int fdout, char* filename, char option){
    /*Define variables*/
    int fdin;
    struct Header* head;
    int lengths[] = [100,8,8,8,12,12,8,1,100,6,2,32,32,8,8,155];
    int entries = 16;

    /*Create header object*/
    head = create_header(filename, option);

    /*Write header variables in order to file*/
    write(fdout, head, 512);
    /*
    write(fdout, head->name, lengths[0]);
    write(fdout, head->mode, lengths[1]);
    write(fdout, head->uid, lengths[2]);
    write(fdout, head->gid, lengths[3]);
    write(fdout, head->size, lengths[4]);
    write(fdout, head->mtime, lengths[5]);
    write(fdout, head->chksum, lengths[6]);
    write(fdout, head->typeflag, lengths[7]);
    write(fdout, head->linkname, lengths[8]);
    write(fdout, head->magic, lengths[9]);
    write(fdout, head->version, lengths[10]);
    write(fdout, head->uname, lengths[11]);
    write(fdout, head->gname, lengths[12]);
    write(fdout, head->devmajor, lengths[13]);
    write(fdout, head->devminor, lengths[14]);
    write(fdout, head->prefix, lengths[15]);
    */

    free(head);

}