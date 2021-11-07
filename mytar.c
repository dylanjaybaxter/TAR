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
#include<string.h>
#include<grp.h>
#include<pwd.h>
#include<sys/stat.h>
#include<stdlib.h>
#include"archive.h"
#include"arch_helper.h"
#define CREATE 0x01
#define PRINT 0x02
#define EXTRACT 0x04
#define VERBOSE 0x08
#define FILENAME 0x10
#define STRICT 0x20

<<<<<<< HEAD

int main(int argc, char* const argv[]){
    char* opts = argv[0];
    uint16_t optMask = 0;
    char opt;
    int i;
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
=======
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

char *octalConvert(unsigned int n, char *octal[]){
    /* Takes a char pointer of a given size and creates an octal number
     * from the given integer.  */
    int i = 0;
    while (n != 0){
        octal[0][i] = 48 + (n % 8);
        n = n / 8;
        i++;   
    } 
    int length = i;
    int j;
    int templen = strlen(octal[0]); 
    char *temp = (char*) malloc(templen * sizeof(char));
    strcpy(temp, octal[0]);
    for (j = length - 1, i = 0; i < length; i++, j--){
        octal[0][i] = temp[j];
    }
    free(temp);
    return octal[0];
}

void init(struct Header *head){
    /* Initializes the header */
    char smallOct[8] = {0};
    char bigOct[12] = {0};
    strcpy(head->mode, smallOct);
    strcpy(head->uid, smallOct);
    strcpy(head->gid, smallOct);
    strcpy(head->size, bigOct);
    strcpy(head->mtime, bigOct);
    strcpy(head->chksum, smallOct);
    head->typeflag = '0';
    strcpy(head->name, "0");
    strcpy(head->linkname, "0");
    strcpy(head->magic, "ustar");
    head->version[0] = '0';
    head->version[1] = '0';
    strcpy(head->uname, "0");
    strcpy(head->gname, "0");
    strcpy(head->devmajor, smallOct);
    strcpy(head->devminor, smallOct);
    strcpy(head->prefix, "0");
}

struct Header *create_header(char *fileName, char option){
    /* Takes the given file name and creates a header. It also takes in an
     * option variable to check if the Strict mode has been set */
    struct Header *head = (struct Header *) malloc(sizeof(struct Header));
    char smallOct[8] = {0}; /* Max Octal Size for multiple vars */
    char bigOct[12] = {0}; /* Max OCtal Size for larger vars */
    init(head);
    int length = strlen(fileName);
    int i;
    int index;

    struct stat file;
    lstat(fileName, &file);
    
    strcpy(head->mode, octalConvert(file.st_mode, smallOct));
    strcpy(head->uid, octalConvert(file.st_uid, smallOct));
    strcpy(head->gid, octalConvert(file.st_gid, smallOct));
    strcpy(head->size, octalConvert(file.st_size, bigOct));
    strcpy(head->mtime, octalConvert(file.st_mtime, bigOct));
    
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(file.st_gid);
    pwd = getpwuid(file.st_uid);
    strcpy(head->uname, pwd->pw_name);
    strcpy(head->gname, grp->gr_name);
>>>>>>> 7bc562016dbd5619b27a007859ba843968c39e46

    /*Initialize Variables for destination and assign*/
    char* dest;
    if(optMask & FILENAME){
        dest = argv[1];
    }else{
        printf("Absent f not supported");
        exit(EXIT_FAILURE);
    }
<<<<<<< HEAD
=======
    return head;
}

>>>>>>> 7bc562016dbd5619b27a007859ba843968c39e46

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
        createArchive(dest, paths, pathCount, optMask);
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
