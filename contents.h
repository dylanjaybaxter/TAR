#include "extract.h"

void printContents(char *fileName, char *archive, unsigned int options);

void permissions(mode_t mode, char* perm, struct Header* head);