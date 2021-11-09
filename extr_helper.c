#include "extr_helper.h"
#include <math.h>
#include <stdlib.h>

uint32_t extract_special_int(char *where, int len){
    int32_t val = -1;
    if((len >= sizeof(val)) && (where[0] & 0x80)){
        val = *(int32_t *)(where+len-sizeof(val));
        val = ntohl(val);
    }
    return val;
}


double pow2(double base, double power){
    int i;
    int sum = 1;
    for(i = 1; i <= power; i++){
        sum *= base;
    }
    return sum;
}

int checkpre(char* pre, char* str){
    /*Strip file contents*/
    int i;
    int prelen = strlen(pre);
    for(i=0;i<prelen;i++){
        if(pre[i] != str [i]){
            return 0;
        }
    }
    if(pre[prelen-1] != '/'){
        return 0;
    }
    return 1;
}

int unoctal(char *octal){
    int len = strlen(octal);
    int i;
    double power = 0;
    int sum = 0;
    for (i = len - 1; i != -1; i--){
        if (octal[i] != 0){
            int num = octal[i] - 48;
            double powSum = pow2(8.0, power);
            sum += num * powSum;
            power++;
        }
    }
    return sum;
}

void ensureDir(char* path){
    int len = strlen(path);
    char dirPath[256] = {'\0'};
    int i;
    int ind;
    for(i = len-1; i>-1; i--){
        if(path[i] == '/'){
            ind = i;
            break;
        }
    }
    for(i=0;i<=ind;i++){
        dirPath[i] = path[i];
        if((path[i] == '/') && (i!=0)){
            if (mkdir(dirPath, 0777) == -1){
                if(errno != EEXIST){
                    perror(dirPath);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
