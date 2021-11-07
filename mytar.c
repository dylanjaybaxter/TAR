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
  return 0;
}
