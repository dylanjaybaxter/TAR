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
    /* Takes in a given file name, archive, and option mask. 
     * Prints the contents of the archive
     */
    int arch;
    /* Open the archive */
    if (-1==(arch = open(archive, O_RDONLY, 0666))){
        perror(archive);
        exit(EXIT_FAILURE);
    }
    /* Initialize variables */
    char buffer[512];
    struct Header *head;
    int numblocks = 0;
    int readsize = 0;
    int checkVal = 0;
    int endblock = 0;
    int i;
    /*Read through the entire archive to find what you are looking for */
    while((readsize = read(arch, buffer, 512)) > 0){
        /* If a header is encountered set the header pointer */
        if (numblocks == 0){
            head = (struct Header *)(buffer);
            checkVal = checksum(head);
            if(checkVal == -1){
                fprintf(stderr,"INVALID HEADER\n");
                return;
            }
            if (checkVal == 0){
                endblock++;
                if(endblock < 1){
                    return;
                }
            }
            /* Get the size and check how many blocks
             * are required for the file
             */
            int size = unoctal(head->size);
            if((size % 512) == 0){
                numblocks = size / 512;
            }
            else{
                numblocks = (size / 512) + 1;
            }
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
            if (options & VERBOSE){
                /* Check if verbose option is set */
                if(!(strcmp(fname, fileName)) || checkpre(fileName, fname)
                || (options & ALLFLAG)){
                    /* Check if you run into a file that you want to print */
                    if(strlen(fname)){
                        int size = unoctal(head->size);
                        time_t mtime = unoctal(head->mtime);
                        mode_t mode = unoctal(head->mode);
                        char buf[11] = {'\0'};
                        char timBuf[18];
                        permissions(mode, buf, head);
                        strftime(timBuf, 18, "%Y-%m-%d %H:%M",
                        localtime(&mtime));
                        /* Convert the mode into a string
                         * Convert the time_t into a readable format
                         */
                        printf("%s %s/%s %14d %s %s\n", buf, head->uname,
                            head->gname, size, timBuf, fname);
                    }
                }
            }
            else{
                if(!(strcmp(fname, fileName)) || checkpre(fileName, fname)
                || (options & ALLFLAG)){
                    /*If verbose is not set just print the name of the file */
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
    /*Takes in the mode of the file, a buffer to input into and the header
     * from the archive. Converts the mode into a string representation
     * for easier readability.
     */
    int i;
    for(i=0;i<11;i++){
        perm[i] = '-';
    }
    /* Check typeflag to determine the first char of the string */
    if ((head->typeflag) == '5'){
        perm[0] = 'd';
    }
    if ((head->typeflag) == '2'){
        perm[0] = 'l';
    }
    /* Check all user permissions */
    if (mode & S_IRUSR){
        perm[1] = 'r';
    }
    if (mode & S_IWUSR){
        perm[2] = 'w';
    }
    if (mode & S_IXUSR){
        perm[3] = 'x';
    }
    /* Check all group permissions */
    if (mode & S_IRGRP){
        perm[4] = 'r';
    }
    if (mode & S_IWGRP){
        perm[5] = 'w';
    }
    if (mode & S_IXGRP){
        perm[6] = 'x';
    }
    /* Check all other permissions */
    if (mode & S_IROTH){
        perm[7] = 'r';
    }
    if (mode & S_IWOTH){
        perm[8] = 'w';
    }
    if (mode & S_IXOTH){
        perm[9] = 'x';
    }
    /* NULL terminate the string */
    perm[10] = 0;
}
