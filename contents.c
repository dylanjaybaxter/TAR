#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <utime.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include "contents.h"
#include "extr_helper.h"

void printContents(char *fileName, char *archive, unsigned int options){
    int arch;
    if (-1==(arch = open(archive, O_RDONLY, 0666))){
        perror(archive);
        exit(EXIT_FAILURE);
    }
    char buffer[512];
    struct Header *head;
    int numblocks = 0;
    int readsize = 0;
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
            char fname[257] = {0};
            char delim[2] = "/\0";
            strcpy(fname, head->prefix);
            if(strlen(fname)){
                strcat(fname, delim);
            }
            strncat(fname, head->name, 256);
            if (options & VERBOSE){
                if(!(strcmp(fname, fileName)) || checkpre(fileName, fname)
                || (options & ALLFLAG)){
                    if(strlen(fname)){
                        int size = unoctal(head->size);
                        time_t mtime = unoctal(head->mtime);
                        mode_t mode = unoctal(head->mode);
                        char buf[11] = {'\0'};
                        char timBuf[18];
                        permissions(mode, buf, head);
                        strftime(timBuf, 18, "%Y-%m-%d %H:%M",
                        localtime(&mtime));
                        printf("%s %s/%s %14d %s %s\n", buf, head->uname,
                            head->gname, size, timBuf, fname);
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

void permissions(mode_t mode, char* perm, struct Header* head){
    int i;
    for(i=0;i<11;i++){
        perm[i] = '-';
    }
    if ((head->typeflag) == '5'){
        perm[0] = 'd';
    }
    if ((head->typeflag) == '2'){
        perm[0] = 'l';
    }
    if (mode & S_IRUSR){
        perm[1] = 'r';
    }
    if (mode & S_IWUSR){
        perm[2] = 'w';
    }
    if (mode & S_IXUSR){
        perm[3] = 'x';
    }
    if (mode & S_IRGRP){
        perm[4] = 'r';
    }
    if (mode & S_IWGRP){
        perm[5] = 'w';
    }
    if (mode & S_IXGRP){
        perm[6] = 'x';
    }
    if (mode & S_IROTH){
        perm[7] = 'r';
    }
    if (mode & S_IWOTH){
        perm[8] = 'w';
    }
    if (mode & S_IXOTH){
        perm[9] = 'x';
    }
    perm[10] = 0;
}
