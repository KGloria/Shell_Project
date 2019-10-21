/* p2.h - header file for the p2.c() used in
   CS570 Fall 2019
   San Diego State University
*/

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

//Checks the parsing job done by Parse
void checkPar(char *argptr[MAXITEM], char parameters[][STORAGE]);

//Seperates given input from stdin/a file and works as a lexical analysis
//to correctly identify words, metacharacters, flags.
int parse(char *s, char *argptr[MAXITEM], char parameters[][STORAGE]);

//Function to execute given commands by the user.
void execcmd();

//Check if a file exists currently to prevent errors/overwritting
int file_exists(const char *file_name);

//set up I/O redirection (pipe is probably next)
int redirectSetUp();

//Variation of Parse for specific cases
int parseInputFile();

//Place the data back in program.
void copyBack();

//Copy flags, arrays, arguments.
void copyOver();

/*A larger and more in depth descition of these functions can be found in 
the p2.c file.  There are bugs.
*/