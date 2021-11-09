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
#include <dirent.h>
#include <limits.h>
#include "archive.h"


struct Header* init_Header(){
    struct Header* head =(struct Header*)calloc(1,sizeof(struct Header));
    /* Initializes the header */
    char smallOct[8] = {'\0'};
    char bigOct[12] = {'\0'};
    strcpy(head->mode, smallOct);
    strcpy(head->uid, smallOct);
    strcpy(head->gid, smallOct);
    strcpy(head->size, bigOct);
    strcpy(head->mtime, bigOct);
    strcpy(head->chksum, smallOct);
    head->typeflag = '0';
    strcpy(head->name, "0");
    fillArray(head->linkname, 0, 100);
    strcpy(head->magic, "ustar");
    head->version[0] = '0';
    head->version[1] = '0';
    fillArray(head->uname, 0,32);
    fillArray(head->gname, 0,32);
    strcpy(head->devmajor, smallOct);
    strcpy(head->devminor, smallOct);
    fillArray(head->prefix, 0, 100);
    fillArray(head->padding, 0, 12);
    return head;
}

struct Header* create_header(char *fileName, char option){
    /* Takes the given file name and creates a header. It also takes in an
     * option variable to check if the Strict mode has been set */
    struct Header* head = init_Header();
    char smallOct[8] = {0}; /* Max Octal Size for 8 byte vars */
    char bigOct[12] = {0}; /* Max Octal Size for 12 byte vars */
    int length = strlen(fileName);
    int i;
    int breakInd;
    int checksum = 0;

    /*Stat file*/
    struct stat file;
    lstat(fileName, &file);

    /*Write Name and Prefix*/
    char name[100] = {'\0'};
    char pre[155] = {'\0'};
    char fileTemp[256] = {'\0'};

    /*Copy Filename into temp list*/
    if(length < 256){
        strncpy(fileTemp,fileName,length);
    }else{
       perror("File name is too long");
       exit(EXIT_FAILURE);
    }

    /*Append slash to directories*/
    if(S_ISDIR(file.st_mode)){
        if(length<255){
            fileTemp[length] = '/';
        }
        else{
           perror("File name is too long");
           exit(EXIT_FAILURE);
        }
        length++;
    }

    /*Read filname into buffers*/
    if (length <= 256 || (option & STRICT)){
        if(length > 100){
            i = length-100;
            for (; i < length; i++){
                if(fileTemp[i] == '/'){
                    breakInd = i;
                    break;
                }
            }
            for(i=0;i<breakInd;i++){
                pre[i] = fileTemp[i];
            }
            i++;
            for(;i<length;i++){
                name[i-breakInd-1] = fileTemp[i];
            }
        }
        else{
            if(length < 100){
                name[length] = '\0';
            }
            strcpy(name, fileTemp);
        }
        strcpy(head->name, name);
        strcpy(head->prefix, pre);
    }
    else{
       perror("File name is too long");
       exit(EXIT_FAILURE);
    }

    /*Write mode*/
    memset(smallOct, '0', 8);
    strcpy(head->mode, octalConvert((file.st_mode & 0x1FF), smallOct, 8));

    /*Convert usable and write uid*/
    if(insert_special_int(head->uid, 8, file.st_uid)){
        perror("insert_special_int");
        exit(EXIT_FAILURE);
    }
    memset(smallOct, '0', 8);
    /*strcpy(, usableUID);*/

    /*Write gid*/
    memset(smallOct, '0', 8);
    strcpy(head->gid, octalConvert(file.st_gid, smallOct, 8));

    /*See Size at dependent*/

    /*Write mtime*/
    memset(bigOct, 0, 12);
    strcpy(head->mtime, octalConvert(file.st_mtime, bigOct, 12));

    /*See Checksum at End*/
    /*See TypeFlag at dependent*/
    /*See Linkname at dependent*/
    /*See magic num & version in init*/

    /*Write Group and Password*/
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(file.st_gid);
    pwd = getpwuid(file.st_uid);
    strcpy(head->uname, pwd->pw_name);
    strcpy(head->gname, grp->gr_name);

    /*No Dev Major/Minor*/
    /*See prefix at beginning*/

    /*Type Dependant Fields*/
    if(S_ISREG(file.st_mode)){
        head->typeflag = '0';
        memset(bigOct, '\0', 8);
        strcpy(head->size, octalConvert(file.st_size, bigOct, 12));
    }
    else if(S_ISDIR(file.st_mode)){
        head->typeflag = '5';
        memset(bigOct, '0', 8);
        strcpy(head->size, octalConvert(0, bigOct, 12));
    }
    else if(S_ISLNK(file.st_mode)){
        head->typeflag = '2';
        struct stat sb;
        if(stat(fileName, &sb) == -1){
            perror("STAT");
            exit(EXIT_FAILURE);
        }
        readlink(fileName, head->linkname, 100);
        memset(bigOct, '0', 8);
        strcpy(head->size, octalConvert(0, bigOct, 12));
    }

    /*Count Characters in header and write to checksum*/
    unsigned char* countPt = (unsigned char*)head;
    memset(head->chksum, ' ', 8);
    checksum = 0;
    memset(smallOct, '0', 8);
    for(i=0;i<BLOCK_SIZE;i++){
        /*If offset has a value and is not in checksum
         && ((offset < 148) || (offset > 155))*/
        checksum = checksum+*countPt;
        countPt++;
    }
    strcpy(head->chksum, octalConvert(checksum, smallOct, 8));

    return head;
}

void createArchive(char* dest, char** paths, int pathCount, int options){
    /*Define Variables*/
    int i;
    int fd;

    /*Open New TAR File in destination*/
    if(DEBUG){
        printf("Opening Dest: %s\n", dest);
    }
    if((fd = open(dest, O_CREAT|O_TRUNC|O_WRONLY, 0666)) == -1){
        perror("Open destination");
        exit(EXIT_FAILURE);
    }

    /*For each path specified*/
    for(i=0;i<pathCount;i++){
        /*Recursively write each into file*/
        writeRecur(fd, paths[i], options);
    }

    writePad(fd);

    if(close(fd)){
        perror("Close Dest");
        exit(EXIT_FAILURE);
    }

}


void writeRecur(int fd, char* path, int options){
    /*Open current path*/
    if(options & VERBOSE){
        printf("%s\n", path);
    }
    struct stat sb;
    DIR* d;
    struct dirent* e;
    char newpath[MAX_PATH_SIZE];
    newpath[0] = '\0';
    int flen = 0;
    int plen = 0;

    /*Stat the current target*/
    if(DEBUG){
        printf("Recursive Statting: %s\n", path);
    }
    if(lstat(path, &sb) == -1){
        /*perror("Recursive Stat");*/
        return;
    }

    /*If path is a file, write contents to TAR*/
    if(S_ISREG(sb.st_mode)){
        if(DEBUG){
            printf("Writing file: %s\n", path);
        }
        /*Write Header*/
        writeheader(fd, path, options);

        /*Write Body*/
        writebody(fd, path);

    }

    /*If the items is a symlink*/
    if(S_ISLNK(sb.st_mode)){
        /*Write Header*/
        writeheader(fd, path, options);

    }

    /*If path is a directory, write contents to file
    Then enter Recursively*/
    if(S_ISDIR(sb.st_mode)){
        /*Write Header*/
        writeheader(fd, path, options);

        /*Write Body*/
        writebody(fd, path);

        /*Open directory*/
        if(DEBUG){
            printf("Opening Dir: %s\n", path);
        }
        if((d = opendir(path)) == NULL){
            perror("Open Dir");
            exit(EXIT_FAILURE);
        }

        /*For every file in the directory*/
        while((e = readdir(d))){
            /*Check that new length does not exceed max*/
            if(*(e->d_name) != '.'){
                flen = strlen(e->d_name);
                plen = strlen(path);
                if((flen+plen) < MAX_PATH_SIZE){
                    /*Copy the string to the newpath buffer*/
                    strcpy(newpath, path);

                    /*Add the name to the path*/
                    strncat(newpath, "/" , 2);
                    strncat(newpath, e->d_name, flen+1);

                    /*Explore the entry*/
                    writeRecur(fd, newpath, options);
                }
                else{
                    printf("Max path size reached\n");
                }
            }
        }
        /*For every entry in the directory*/
        while((e = readdir(d))){
            /*Check that new length does not exceed max*/
            if(*(e->d_name) != '.'){
                flen = strlen(e->d_name);
                plen = strlen(path);
                if((flen+plen) < MAX_PATH_SIZE){
                    /*Copy the string to the newpath buffer*/
                    strcpy(newpath, path);

                    /*Add the name to the path*/
                    strncat(newpath, "/" , 2);
                    strncat(newpath, e->d_name, flen+1);

                    /*Explore the entry*/
                    writeRecur(fd, newpath, options);
                }
                else{
                    printf("Max path size reached\n");
                }
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
    char readBuf[BLOCK_SIZE];
    int fdin;
    int readSize;
    int i;

    /*Open read file*/
    if(-1==(fdin = open(path, O_APPEND, 0666))){
        perror("Body Open");
        exit(EXIT_FAILURE);
    }

    /*While 512 bye blocks exist in read file*/
    while(((readSize = read(fdin, readBuf, 512))>0)){
        /*If if on last block, pad with 0's*/
        if(readSize < BLOCK_SIZE){
            for(i=readSize;i<(BLOCK_SIZE);i++){
                readBuf[i] = '\0';
            }
        }
        /*Write block*/
        if(DEBUG){
            printf("Writing Block of Size %d to %d\n", readSize,fdout);
        }
        if(write(fdout, readBuf, BLOCK_SIZE)==-1){
            perror("Body Write");
            exit(EXIT_FAILURE);
        }
    }

    /*Close read file*/
    if((close(fdin) == -1)){
        perror("Body Close");
        exit(EXIT_FAILURE);
    }

}

void writeheader(int fdout, char* filename, int option){
    /*Define variables*/
    struct Header* head;

    /*Create header object*/
    head = create_header(filename, option);

    /*Write header variables in order to file*/
    write(fdout, head, 512);

    free(head);
}


void writePad(int fd){
    /*Define variables*/
    char readBuf[BLOCK_SIZE] = {'\0'};
    /*Write block*/
    if(DEBUG){
        printf("Writing Padding Block to %d\n", fd);
    }
    if(write(fd, readBuf, BLOCK_SIZE)==-1){
        perror("Pad Write");
        exit(EXIT_FAILURE);
    }
    if(write(fd, readBuf, BLOCK_SIZE)==-1){
        perror("Pad Write");
        exit(EXIT_FAILURE);
    }
}