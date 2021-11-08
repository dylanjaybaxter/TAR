/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: extract.c
Description: This file contains functions necessary for extraction
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include "extract.h"


int giveExecute(mode_t e_mode){
    /* Takes a given filename and gives execute to everyone
     * if at least on person has execute permissions
     * returns 0 on success and -1 on failure */
    if(e_mode & S_IXUSR ||
        e_mode & S_IXGRP ||
        e_mode & S_IXOTH){

        return 0;
    }
    return -1;
}

int checksum(struct Header *head){
    /* Returns 1 on valid header
     * Returns -1 on invalid header/blank block
     Returns 0 on null block
     */
     char smallOct[8] = {'\0'};
     unsigned char* countPt = (unsigned char*)head;
     int realsum = unoctal(head->chksum);
     int checksum = 0;
     memset(head->chksum, ' ', 8);
     memset(smallOct, '0', 8);
     int i;
     for(i=0;i<BLOCK_SIZE;i++){
         checksum = checksum+*countPt;
         countPt++;
     }
     strcpy(head->chksum, octalConvert(realsum, smallOct, 8));
     if(checksum == 256){
         return 0;
     }
     else if(checksum == realsum){
         return 1;
     }
     else{
         return -1;
     }
}

void extract_file(char *path, struct Header *head, int fdHead){
    /* Takes a path name of a file and the header for said file and
     * extracts its contents from the archive into a new file with the
     * given path
     */
    mode_t e_mode = unoctal(head->mode);
    off_t e_size = unoctal(head->size);
    time_t e_mtime = unoctal(head->mtime);
    /*struct utimbuf oldtime;
    oldtime.modtime = e_mtime;
    utime(path, oldtime);*/
    char buffer[512] = {0};
    if (giveExecute(e_mode) == 0){
        int i = e_size;
        int fd = open(path, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
        while (i >= 512){
            read(fdHead, buffer, 512);
            write(fd, buffer, 512);
            i -= 512;
        }
        read(fdHead, buffer, 512);
        write(fd, buffer, i);
    }
    else{
        int fd = open(path, O_WRONLY | O_CREAT, 0666);
        char buffer[512] = {0};
        read(fdHead, buffer, 512);
        write(fd, buffer, 512);
    }
}

void extract_directory(char *path, struct Header *head){
    /* Takes a path for a given directory and extracts the directory from
     * the given archive *
     */
    mode_t e_mode = unoctal(head->mode);
    if (mkdir(path, e_mode) == -1){
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
}

void extract(char *fileName, char *archive){
    /* Searches through the archive for the given file name.
     * If it exists it attempts to extract the file.
     */
    int fd = open(archive, O_RDONLY, 0666);
    char buffer[512];
    read(fd, buffer, 512);
    struct Header *head;
    int numblocks = 0;
    int readsize = 0;
    int checkVal = 0;
    int endblock = 0;
    while((readsize = read(fd, buffer, 512)) > 0){
        if (numblocks == 0){
            head = (struct Header *)(buffer);
            /*Check for null blocks and invalid headers*/
            checkVal = checksum(head);
            if(checkVal == -1){
                printf("INVALID HEADER\n");
                exit(EXIT_FAILURE);
            }
            if(checkVal == 0){
                endblock++;
                if(endblock < 1){
                    return;
                }
            }
            int size = unoctal(head->size);
            /* If there is a remainder we need and entire 512 byte block
            */
            if ((size % 512) == 0){
                numblocks = size / 512;
            }
            else{
                   numblocks = (size / 512) + 1;
            }
            char fname[256] = {0};
            strcpy(fname, head->prefix);
            strcat(fname, head->name);
            if (fname == fileName || strstr(fname, fileName)){
            /*Check if the fileNames match */
                if (head->typeflag == '5'){
                /* Checks if the file is a directory */
                    extract_directory(fname, head);
                }
                else if (head->typeflag == '0'){
                /* Checks if the file is a REG file*/
                    extract_file(fname, head, fd);
                }
            }
        }
        else{
            numblocks--;
        }
    }
}

