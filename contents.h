#include "extract.h"

void printContents(char **fileNames, int pathcount,
     char *archive, unsigned int options);

void permissions(mode_t mode, char* perm, struct Header* head);