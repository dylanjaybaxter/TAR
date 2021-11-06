/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark05)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<getopt.h>
#include<unistd.h>
<<<<<<< HEAD
#include<string.h>
#include<grp.h>
#include<pwd.h>
#include<sys/stat.h>
#include<stdlib.h>
=======
#include"archive.h"
#include"arch_helper.h"
>>>>>>> 5feb1ba7de8e6891d657ab47ae20ed1c92e4a94d
#define CREATE 0x01
#define PRINT 0x02
#define EXTRACT 0x04
#define VERBOSE 0x08
#define FILENAME 0x10
#define STRICT 0x20

struct Header{
    /* Header structure
     * Everything is chars because they are either names or strings
     * that portray octal number */
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
};

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

void init(struct Header *head){
    /* Initializes the header */
    char *smallOct = (char *) malloc(8);
    char *bigOct = (char *) malloc(12);
    head->mode = octalConvert(0, smallOct);
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
    init(head);
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


int main(int argc, char* const argv[]){
    uint16_t optMask = 0;
    int i;

    /*Iterate through options and assign flags*/
<<<<<<< HEAD
    while((opt = getopt(argc, argv,":cxtvfS:")) != -1){
        /**/
        if(opt == 'c'){
            optMask = CREATE;
        }
        /*check for missing number*/
            else if(opt == 't'){
            optMask = PRINT;
        }
        else if(opt == 'x'){
            optMask = EXTRACT;
        }
        else if(opt == 'v'){
            optMask = optMask|VERBOSE;
        }
        else if(opt == 'f'){
            optMask = optMask|FILENAME;
        }
        else if(opt == 'S'){
            optMask = optMask|STRICT;
        }

        for(;optind < argc; optind++){
        
=======
    char* opts = argv[0];
    char opt;
    for(i=0;i<(int)strlen(opts);i++){
        opt = opts[i];
      /*Flag create, stop if print or extrac*/
      if(opt == 'c'){
          optMask = CREATE;
      }
      /*flag table*/
      else if(opt == 't'){
          optMask = PRINT;
      }
      /*Flag Extract*/
      else if(opt == 'x'){
          optMask = EXTRACT;
      }
      /*Flag verbose*/
      else if(opt == 'v'){
          optMask = optMask|VERBOSE;
      }
      /*flag filename argument*/
      else if(opt == 'f'){
          optMask = optMask|FILENAME;
      }
      /*flag strict compilation*/
      else if(opt == 'S'){
          optMask = optMask|STRICT;
      }
      /*Reject any other argument*/
      else{
          printf("Usage: tar [txvfS] [dest] [paths to tar]");
          exit(EXIT_FAILURE);
      }
  }

    /*Initialize Variables for destination and assign*/
    char* dest;
    if(optMask & FILENAME){
        dest = argv[1];
    }else{
        printf("Absent f not supported");
        exit(EXIT_FAILURE);
    }

    /*Initialize vars for actions*/
    char* path;
    char* paths[argc-2];
    int pathCount =0;

    /*For the remainder of arguments, Perform action*/
    if(optMask & CREATE){
        for(i=2;i < argc; i++){
            paths[i] = argv[i];
            pathCount++;
        }
        createArchive(dest, paths, pathCount);
    }
    else if(optMask & PRINT){
        path = argv[2];
        /*Print the contents of the tar file*/
        if(isTAR(path)){
            printTAR(1,path);
        }
        else{
            printf("This is not a TAR file");
            exit(EXIT_FAILURE);
>>>>>>> 5feb1ba7de8e6891d657ab47ae20ed1c92e4a94d
        }
    }
    else if(optMask & EXTRACT){
        path = argv[2];
        /*Check if the file is a TAR file*/
        if(isTAR(path)){
            extractArchive(dest, path);
        }
        else{
            printf("This is not a TAR file");
            exit(EXIT_FAILURE);
        }
    }
    else{
        printf("You never should have come here\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
