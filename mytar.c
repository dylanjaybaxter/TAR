/*
CPE 357 Asgn4
Author: Dylan Baxter (dybaxter), Van Park (vpark)
File: mytar.c
Description: This file contains main functionality for home
implement of mytar.c
*/
#include<stdio.h>
#include<stdint.h>
#include<getopt.h>
#include<unistd.h>
#define CREATE 0x01
#define PRINT 0x02
#define EXTRACT 0x04
#define VERBOSE 0x08
#define FILENAME 0x10
#define STRICT 0x20

int main(int argc, char* const argv[]){
    uint16_t optMask = 0;
    int opt;

    /*Iterate through options and assign flags*/
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


    }
    return 0;
}