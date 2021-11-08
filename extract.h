#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "archive.h"
#include "extr_helper.h"
#include "arch_helper.h"

int giveExecute(mode_t e_mode);

void extract_file(char *path, struct Header *head, int fd);

void extract_directory(char *path, struct Header *head);

int checksum(struct Header *head);

void extract(char *fileName, char *archive);


