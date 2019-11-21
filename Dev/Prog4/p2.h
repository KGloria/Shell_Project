
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <tcl.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#include "getword.h"
#define MAXITEM 100 /* Max Number of words per line */
#define STORAGE 255

void checkPar(char *argptr[MAXITEM], char parameters[][STORAGE]);

int parse(char *s, char *argptr[MAXITEM], char parameters[][STORAGE]);

void execcmd();

int file_exists(const char *file_name);

int redirectSetUp();