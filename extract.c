#include "extract.h"
#include "archive.h"

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

void extract_file(char *path, struct Header *head, char *archive){
    /* Takes a path name of a file and the header for said file and 
     * extracts its contents from the archive into a new file with the 
     * given path 
     */
    mode_t e_mode = unoctal(head->mode);
    off_t e_size = unoctal(head->size);
    time_t e_mtime = unoctal(head->mtime);
    struct utimbuf oldtime;
    oldtime.modtime = e_mtime;
    utime(path, oldtime);

    int fdHead = open(archive, O_RONLY, 0666);
    if (giveExecute(e_mode) = 0){
        int i; 
        int fd = open(path, O_CREAT, S_RWXUSR | S_RWXGRP | S_RWXOTH);
        for (i = e_size; i >= 512; i - 512;){
            char buffer[512] = {0};
            read(fdHead, buffer, 512);
            write(fd, buffer, 512);
        }
        read(fdHead, buffer, 512);
        write(fd, buffer, i);
    }
    else{
        int fd = open(path, O_CREAT, S_RWUSR | S_RWGRP | S_RWOTH);
        char buffer[512] = {0};
        read(fdHead, buffer, 512);
        write(fd, buffer, 512);
}

void extract_directory(char *path, struct Header *head, char *archive){
    /* Takes a path for a given directory and extracts the directory from 
     * the given archive *
     */
    mode_t e_mode = unoctal(head->mode);
    if (mkdir(path, e_mode) = -1){
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
}

int checksum(header *head){
    /* Returns 1 on valid header
     * Returns 0 on invalid header/blank block
     */
}

void extract(char *fileName, char *archive){
    /* Searches through the archive for the given file name.
     * If it exists it attempts to extract the file. 
     */
    int fd = open(archive, O_RONLY, 0666);
    char buffer[512];
    read(fd, buffer, 512);
    struct Header *head;
    int numblocks = 0;
    while(readsize = read(fd, buffer, 512)){
        if (numblocks == 0){
            head = (struct Header *)(buffer);
            int size = unoctal(head->size);
            /* If there is a remainder we need and entire 512 byte block
            */
            if ((size % 512) == 0){
                numblocks = size / 512;
            }
            else{
                   numblocks = (size / 512) + 1;
            }
            char fname[256] = strcat(head->prefix, head->name);
           if (fname == fileName || strstr(fname, filename)){ 
            /*Check if the fileNames match */
                if (head->typflag == '5'){ 
                /* Checks if the file is a directory */
                    exctract_directory(fname, head, archive);
                }
                else if (head->typeflag == '0'){ 
                /* Checks if the file is a REG file*/
                    exract_file(fname, head, archive);
                }
            }    
        }
        else{
            numblocks--;
        }
    }
}
