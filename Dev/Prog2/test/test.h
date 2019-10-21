#include <stdio.h>
#include "getword.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>

#define MAXITEM 100
#define MAXSTORAGE 25500


int parse();

void myhandler();