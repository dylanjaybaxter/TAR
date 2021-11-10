/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark05)
File: extract.c
Description: This file contains functions necessary for extraction from
tar archives
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <utime.h>
#include <time.h>
#include <sys/time.h>
#include "extract.h"
#include "extr_helper.h"

int giveExecute(mode_t e_mode){
    /* Takes a given filename and gives execute to everyone
     * if at least on person has execute permissions
     * returns 0 on success and -1 on failure */
    if((e_mode & S_IXUSR) ||
        (e_mode & S_IXGRP) ||
        (e_mode & S_IXOTH)){

        return 1;
    }
    return 0;
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
     if(DEBUG){
         printf("Checksum %s: %d %d\n", head->name, checksum, realsum);
     }
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
    time_t atime = time(NULL);
    struct utimbuf timebuf;
    int i = e_size;
    int fd;
    /*struct utimbuf oldtime;
    oldtime.modtime = e_mtime;
    utime(path, oldtime);*/
    char buffer[512] = {0};
    ensureDir(path);
    if(giveExecute(e_mode)){
        e_mode = 0777;
    }else{
        e_mode = 0666;
    }

    if(e_size > 0){
        if(-1 ==(fd = open(path, O_CREAT|O_TRUNC|O_WRONLY,e_mode))){
            perror(path);
            exit(EXIT_FAILURE);
        }
        if(DEBUG){
            printf("File Descriptor is %d\n", fd);
        }

        while (i >= 512){
            if(-1 == read(fdHead, buffer, 512)){
                perror("Read Body TAR");
                exit(EXIT_FAILURE);
            }
            if(DEBUG){
                printf("Writing Chunk of size %d", 512);
            }
            if(-1 == write(fd, buffer, 512)){
                perror("Write Body");
                exit(EXIT_FAILURE);
            }
            i -= 512;
        }
        if(-1==read(fdHead, buffer, 512)){
            perror("Read Final Block");
            exit(EXIT_FAILURE);
        }
        if(DEBUG){
            printf("Writing Chunk of size %d\n", i);
            printf("Writing: %d %s %d\n", fd, buffer, i);
        }
        if(-1==write(fd, buffer, i)){
            perror("Write Final Block");
            exit(EXIT_FAILURE);
        }
    }
    else{
        fd = 0;
        if(-1 ==(fd = open(path, O_CREAT|O_TRUNC|O_WRONLY, e_mode))){
            perror(path);
            exit(EXIT_FAILURE);
        }
    }

    if(close(fd) == -1){
        perror("Close Body Write");
        exit(EXIT_FAILURE);
    }

    /*Time Stuff*/
    struct stat sb;
    if(stat(path, &sb) == -1){
        perror("Time Stat");
        exit(EXIT_FAILURE);
    }
    atime = sb.st_atime;
    timebuf.actime = atime;
    timebuf.modtime = e_mtime;
    if(-1 == utime(path, &timebuf)){
        perror("Time Stuff");
        exit(EXIT_FAILURE);
    }


}

void extract_link(char *path, struct Header *head){
    /* Takes a path name of a file and the header for said file and
     * extracts its contents from the archive into a new file with the
     * given path
     */
     ensureDir(path);
     if (symlink(head->linkname, path) == -1){
         if(errno == EEXIST){
             return;
         }
         else{
             perror("symlink");
             exit(EXIT_FAILURE);
         }
     }
}

void extract_directory(char *path, struct Header *head){
    /* Takes a path for a given directory and extracts the directory from
     * the given archive *
     */
    mode_t e_mode = unoctal(head->mode);
    if(giveExecute(e_mode)){
        e_mode = 0777;
    }else{
        e_mode = 0666;
    }
    ensureDir(path);
    if (mkdir(path, e_mode) == -1){
        if(errno == EEXIST){
            return;
        }
        else{
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }
}

void extract(char **fileNames, int pathcount,
    char *archive, unsigned int optMask){
    /* Searches through the archive for the given file name.
     * If it exists it attempts to extract the file.
     */
     /*Define Variables*/
     char buffer[512];
     struct Header *head;
     int numblocks = 0;
     int readsize = 0;
     int checkVal = 0;
     int endblock = 0;
     int i = 0;
     int fd;
     /*Open the archive*/
    if (-1==(fd = open(archive, O_RDONLY, 0666))){
        perror(archive);
        exit(EXIT_FAILURE);
    }
    /*For each header in the file*/
    while((readsize = read(fd, buffer, 512)) > 0){
        if(DEBUG){
            printf("Reading block of size %d\n", readsize);
        }
        /*If not a body block*/
        if (numblocks == 0){
            /*Extract the header*/
            head = (struct Header *)(buffer);

            /*Check for null blocks and invalid headers*/
            checkVal = checksum(head);
            if(checkVal == -1){
                perror("INVALID HEADER\n");
                exit(EXIT_FAILURE);
            }
            /*Exit On encountering 2 empty blocks*/
            if(checkVal == 0){
                endblock++;
                if(DEBUG){
                    printf(" End Block %d Encountered \n", endblock);
                }
                if(endblock > 1){
                    if(DEBUG){
                        printf("EOF \n");
                    }
                    return;
                }
            }
            else{
                /*Get Size of body and calculate number of body blocks to skip*/
                int size = unoctal(head->size);

                /* If there is a remainder we need and entire 512 byte block
                */
                if ((size % 512) == 0){
                    numblocks = size / 512;
                }
                else{
                       numblocks = (size / 512) + 1;
                }

                /*Parse Path name*/
                char fname[257] = {0};
                char delim[2] = "/\0";
                for(i=0;i<155;i++){
                    if(head->prefix[i] != '\0'){
                        fname[i] = head->prefix[i];
                    }
                    else{
                        break;
                    }
                }
                if(strlen(fname)){
                    strcat(fname, delim);
                }
                strncat(fname, head->name, 100);

                /*Find type*/
                int extractFlag = 0;
                for(i=0;i<pathcount;i++){
                    if (!(strcmp(fname, fileNames[i]))
                        || checkpre(fileNames[i], fname)){
                            extractFlag = 1;
                            break;
                        }
                }
                if((optMask & ALLFLAG)){
                    extractFlag = 1;
                }

                /*Check if the fileNames match */
                if (extractFlag){
                    if(optMask & VERBOSE){
                        printf("%s\n", fname);
                    }

                    if (head->typeflag == '5'){
                        /* Checks if the file is a directory */
                        if(DEBUG){
                            printf("Extracting directory: %s\n", fname);
                        }
                            extract_directory(fname, head);
                    }
                    else if ((head->typeflag == '0')
                        ||(head->typeflag == '\0')){
                        /* Checks if the file is a REG file*/
                        if(DEBUG){
                            printf("Extracting File: %s\n", fname);
                        }
                            extract_file(fname, head, fd);
                            numblocks = 0;
                    }
                    else if (head->typeflag == '2'){
                        /* Checks if the file is a SYMLNK file*/
                        if(DEBUG){
                            printf("Extracting SYMLNK: %s\n", fname);
                        }
                            extract_link(fname, head);
                            numblocks = 0;
                    }
                }
            }
        }
        else{
            numblocks--;
        }
    }
}

