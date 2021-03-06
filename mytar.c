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
#include"contents.h"


int main(int argc, char* const argv[]){
    char* opts;
    if(argc >= 2){
        opts = argv[1];
    }
    else{
        perror("Usage: tar [txvfS] [dest] [paths to tar]");
        exit(EXIT_FAILURE);
    }
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
          perror("Usage: tar [txvfS] [dest] [paths to tar]\n");
          exit(EXIT_FAILURE);
      }
    }

    /*Initialize Variables for destination and assign*/
    char* dest;
    if((optMask & FILENAME) && argc >= 3){
        dest = argv[2];
    }else{
        perror("Absent f not supported");
        exit(EXIT_FAILURE);
    }

    /*Initialize vars for actions*/
    char* paths[argc-3];
    int pathCount =0;


    /*For the remainder of arguments, Perform action*/
    if(optMask & CREATE){
        if(argc < 4){
            perror("Create Usage: tar [txvfS] [dest] [paths to tar]\n");
            exit(EXIT_FAILURE);
        }
        for(i=3;i < argc; i++){
            paths[i-3] = argv[i];
            pathCount++;
        }
        createArchive(dest, paths, pathCount, optMask);
    }
    else if(optMask & PRINT){
        /*Print the contents of the tar file*/
        if(argc > 3){
            for(i=3;i < argc; i++){
                paths[i-3] = argv[i];
                pathCount++;
            }
            /*Check if the file is a TAR file*/
            if(isTAR(dest, optMask)){
                printContents(paths, pathCount, dest, optMask);
            }
            else{
                perror("This is not a TAR file");
                exit(EXIT_FAILURE);
            }
        }
        else if(argc == 3){
            printContents(paths, pathCount, dest, optMask | ALLFLAG);
        }
        else{
            perror(" Extract Usage: tar [txvfS] [dest] [paths to tar]\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(optMask & EXTRACT){
        if(argc > 3){
            for(i=3;i < argc; i++){
                paths[i-3] = argv[i];
                pathCount++;
            }
            /*Check if the file is a TAR file*/
            if(isTAR(dest, optMask)){
                extract(paths, pathCount, dest, optMask);
            }
            else{
                perror("This is not a TAR file");
                exit(EXIT_FAILURE);
            }
        }
        else if(argc == 3){
            extract(paths, pathCount, dest, optMask | ALLFLAG);
        }
        else{
            perror(" Extract Usage: tar [txvfS] [dest] [paths to tar]\n");
            exit(EXIT_FAILURE);
        }
    }
    else{
        perror("You never should have come here\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
