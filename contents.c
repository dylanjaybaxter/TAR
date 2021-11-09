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
#include "contents.h"
#include "extr_helper.h"

#define CDEBUG 0

void printContents(char *fileName, char *archive, unsigned int options){
    if(CDEBUG){
        printf("Printing %s %s %02x\n", fileName, archive, options);
    }
    int arch;
    if (-1==(arch = open(archive, O_RDONLY, 0666))){
        perror(archive);
        exit(EXIT_FAILURE);
    }
    char buffer[512];
    struct Header *head;
    int numblocks = 0;
    int readsize;
    int checkVal = 0;
    int endblock = 0;
    while((readsize = read(arch, buffer, 512)) > 0){
        if (numblocks == 0){
            head = (struct Header *)(buffer);
            checkVal = checksum(head);
            if(checkVal == -1){
                printf("INVALID HEADER\n");
                exit(EXIT_FAILURE);
            }
            if (checkVal == 0){
                endblock++;
                if(endblock < 1){
                    return;
                }
            }

            int size = unoctal(head->size);
            if((size % 512) == 0){
                numblocks = size / 512;
            }
            else{
                numblocks = (size / 512) + 1;
            }
            char fname[256] = {0};
            strcpy(fname, head->prefix);
            strcat(fname, head->name);
            if (options & VERBOSE){
                if(!(strcmp(fname, fileName)) || checkpre(fileName, fname)
                || (options & ALLFLAG)){
                    if(strlen(fname)){
                        printf("%s %s %s %s %s %s\n", head->mode, head->uname,
                            head->gname, head->size, head->mtime, fname);
                    }
                }
            }
            else{
                if(!(strcmp(fname, fileName)) || checkpre(fileName, fname)
                || (options & ALLFLAG)){
                    if(strlen(fname)){
                        printf("%s\n", fname);
                    }
                }
            }
        }
        else{
            numblocks--;
        }
    }
}
