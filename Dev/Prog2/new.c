/*  Name: Kyle Gloria
    Class: CS570 Operating Systems
    Instructor: John Carroll
    Term: Fall 2019 
    School: San Diego State University
    Due Date: October 4, 2019
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

int inFlag = 0;
int outFlag = 0;
int oAmpFlag = 0;
int ampFlag= 0;
int idx = 0;
int ptridx = 0;

int bang = 0;

int clearFlag = 0;

int argFlag = 0;

struct parseFlags {
    int in;
    int out;
    int oAmp;
    int amp;
};

static void printFlags()
{
    printf("< = %d\n", inFlag);
    printf("> = %d\n", outFlag);
    printf("& = %d\n", ampFlag);
    printf("<& = %d\n", oAmpFlag);
}

static void clearFlags()
{
    int inFlag = 0;
    int outFlag = 0;
    int oAmpFlag = 0;
    int ampFlag= 0;
    idx = 0;
    ptridx = 0;
}

static void checkPar(char *argptr[MAXITEM], char parameters[][STORAGE])
{
        for (int i=0; i < idx; i++)
    {
        printf("parameter -- %s\n", parameters[i]);
    }

    for (int j=0; j < ptridx; j++)
    {
        printf("arg ptr -- %s\n", argptr[j]);
    }
}

static int parse(char *s, char *argptr[MAXITEM], char parameters[][STORAGE])
{
    int c;
    while((c=getword(s)) != 0)
    {
        if (c == 0 && s == '\0') return -2;
        if(((strcmp(s,"!!")) == 0) && clearFlag == 0)
        {
            printf("BANG!!\n");
            bang = 1;
            break;
        }
        if (clearFlag == 0) 
        {
            clearFlags();
            memset(argptr, 0, MAXITEM * sizeof *argptr);
            clearFlag = 1;
        }
        if((c == -1) && (idx == 0)) 
        {
            clearFlag = 0;
            return -1;
        }
        strcpy(parameters[idx++],s);
        if (c != 2048)
        {
            argptr[ptridx++] = parameters[idx - 1];
        }
        if (strcmp("<", s) == 0) inFlag = 1;
        if (strcmp(">", s) == 0) outFlag = 1;
        if (strcmp("&", s) == 0) ampFlag = 1;
        if (strcmp(">&", s) == 0) oAmpFlag = 1;
    }
    clearFlag = 0;
    return 0;
}

void myhandler(int signum)
{
    return;
}

int main (int argc, char *argv[])
{
    char *argptr[MAXITEM];
    char parameters[MAXITEM][STORAGE];
    char s[STORAGE];

    char *prompt = "%1% ";

    int c;

    pid_t child_pid, parent_pid, tpid;
    int child_status;
    parent_pid = getpgrp();

    (void) signal(SIGTERM, myhandler);
    setpgid(parent_pid, 00);

    for (;;)
    {    
        printf("%s", prompt);
        c = parse(s, argptr, parameters);
        if (c == -1) break;
        if (c == -2) printf("Empty\n");
        else {
            if ((strcmp(parameters[0],"cd")) == 0)
            {
                if (parameters[1] != NULL && parameters[2] == NULL){
                    // Check if directory exists
                    DIR * dir = opendir(parameters[1]);
                    if (dir)
                    {
                        // Change directory
                        chdir(parameters[1]); 
                    }   
                    else 
                        fprintf(stderr,"CD has failed.\n");
                }
                else if (parameters[1] == NULL) 
                {
                    chdir(getenv("HOME")); 
                }
            }

            else if ((strcmp(parameters[0],"done")) == 0)
            {
                break;
            }

            else
            {
                fflush(stdout);
                fflush(stderr);
                if ((child_pid=fork()) == -1) {
                    perror("Cannot Fork.");
                }
                if(child_pid == 0) {
                    /* This is done by the child process. */ 
                    execvp(parameters[0], argptr);
                    /* If execvp returns, it must have failed. */
                    fprintf(stderr,"Unknown command\n");
                    exit(0);
                }
                else {
                    /* This is run by the parent.  Wait for the child
                        to terminate. */ 
                    tpid = wait(&child_status); 
                }
            }
        }
    }

    killpg(child_pid, SIGTERM);
    printf("p2 terminated.\n"); //prtintf comes after killpg
    exit(0);
}