/*  Name: Kyle Gloria
    Class: CS570 Operating Systems
    Instructor: John Carroll
    Term: Fall 2019 
    School: San Diego State University
    Due Date: October 9, 2019
*/
#include "p2.h"
#include "getword.h"

#define MAXITEM 100 /* Max Number of words per line */
#define STORAGE 255
#define ZERO_COMMANDS 0
#define FAIL -255
#define SUCCESS 0
#define STRING_EQUALS 0 
#define TRUE 1
#define FALSE 0
#define PARSE_ERROR -2

int inFlag = 0;
int outFlag = 0;
int ampFlag= 0;
int idx = 0;
int ptridx = 0;
int bang = 0;
int clearFlag = 0;
int blank = 1;

int redirectFlag = 0;

int *OUT = NULL;
int *IN = NULL;

char *argptr[MAXITEM];
char parameters[MAXITEM][STORAGE];
char s[STORAGE];

static void printFlags()
{
    printf("< = %d\n", inFlag);
    printf("> = %d\n", outFlag);
    printf("& = %d\n", ampFlag);
}

static void clearFlags()
{
    inFlag = 0;
    outFlag = 0;
    ampFlag= 0;
    idx = 0;
    ptridx = 0;
    IN = NULL;
    OUT = NULL;
    redirectFlag = 0;
}

void myhandler(int signum)
{
    return;
}

int main (int argc, char *argv[])
{
    char *prompt = "%1% ";
    int c;

    pid_t child_pid, parent_pid, tpid;
    int child_status;

    setpgid(0, 0);
    (void) signal(SIGTERM, myhandler);

    while(TRUE)
    {    
        blank = 1;
        bang = 0;
        printf("%s", prompt);
        c = parse(s, argptr, parameters);
        
        //Testing purposes
        //printf("%d ----\n", c);
        //printFlags();
        //checkPar(argptr, parameters);
/*
        printf("%d\n", IN);
        printf("%d\n", OUT);
        printf("arg --- %d\n", argptr[1]);*/

        if (c == -1) break;

        else if (c == -2) 
        {
            fflush(stdout);
            continue;
        }

        else if ((strcmp(parameters[0],"cd")) == 0)
        {
            DIR * dir;
            if (idx > 1){
                // Check if directory exists
                dir = opendir(parameters[1]);
                if (dir)
                {
                    chdir(parameters[1]);
                }  
                else
                {
                    perror("CD failed.");
                }
            }
            else 
            {
                    chdir(getenv("HOME"));
            }
        }

        else if ((strcmp(parameters[0],"!!")) == 0)
        {
            //printf("BANG!\n");
            fflush(stdout);
            fflush(stderr);
        }

        else if ((strcmp(parameters[0],"done")) == 0)
        {
            break;
        }
        else {
            //printf("Trying to exec\n");
            execcmd();
            clearFlags();
        }

    }

    killpg(getpgrp(), SIGTERM);
    printf("p2 terminated.\n"); //prtintf comes after killpg
    exit(0);
}

void checkPar(char *argptr[MAXITEM], char parameters[][STORAGE])
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

void execcmd() {
    pid_t child_pid, tpid;
    int child_status;
    int redirecion_return;

    fflush(stdout);
    fflush(stderr);

    if ((child_pid=fork()) == -1) 
    {
        perror("Cannot Fork.");
        return;
    }

    else if(child_pid == 0) {

        if (redirectFlag)
        {
            redirectSetUp();
        }

        /* This is done by the child process. */ 
        execvp(parameters[0], argptr);
        /* If execvp returns, it must have failed. */
        printf("Unknown command\n");
        exit(0);
    }
    else {
        /* This is run by the parent.  Wait for the child
            to terminate. */ 
        if (!ampFlag)
        { do{
            child_status = wait(NULL); 
            }while(child_status != child_pid);
        }
    }

    if (inFlag) {
        if(0)
        {
            perror("Error in setting up I/O Redirecion.");
            exit(-2);
        }
    }
}

int redirectSetUp()
{
    int fd = 0;
    if(OUT != NULL)
    {
        if (file_exists(OUT) == 0)
        {
            perror("File exists");
            return -1;
        }
        fd = open(OUT,O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR);

        if (fd < 0) 
        {
            perror("Existing file. Cannot overwrite.");
            exit(-2);
        }

        if(dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("Dup2() has failed to redirect output.");
            exit(-4);
        }
        close(fd);
    }

    if(IN != NULL)
    {
        fd = open(IN, O_RDONLY);

        if (fd < 0) 
        {
            perror("Cannot read input.");
            exit(-3);
        }

        if(dup2(fd, STDIN_FILENO) < 0)
        {
            perror("Dup2() has failed to redirect input.");
            exit(-5);
        }
        close(fd);
    }
}

int file_exists(const char *file_name)
{
    return access(file_name, F_OK);
}

int parse(char *s, char *argptr[MAXITEM], char parameters[][STORAGE])
{
    int c;
    int oRedirect = 0;
    int iRedirect = 0;

    while((c=getword(s)) != 0)
    {
        //printf("Find Bus error %d\n", idx);
        blank = 0;
        if(((strcmp(s,"!!")) == 0) && clearFlag == 0 && idx == 0)
        {
            bang = 1;
            clearFlag = 1;
            return 11;
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
        if (c==-1) break;
        strcpy(parameters[idx++],s);
        ampFlag = 0;

        if (c != 2048)
        {
            if (OUT == NULL && oRedirect == 1)
            {
                OUT = parameters[idx - 1];
            }
            else if (IN == NULL && iRedirect == 1)
            {
                IN = parameters[idx - 1];
            }
            else {
                argptr[ptridx++] = parameters[idx - 1];
            }

        }
        if (strcmp("<", s) == 0) 
        {
            if (inFlag == 1) {
                perror("Multiple input redirections detected. Cannot continue.");
                return -1;
            }
            redirectFlag = 1;
            inFlag = 1;
            iRedirect = 1;
        }
        if (strcmp(">", s) == 0) 
        {
            if (outFlag == 1) {
                perror("Multiple output redirections detected. Cannot continue.");
                return -1;
            }
            redirectFlag = 1;
            outFlag = 1;
            oRedirect = 1;
        }
        if (strcmp("&", s) == 0) ampFlag = 1;
        if (strcmp(">&", s) == 0)
        {
            if (outFlag == 1) {
                perror("Multiple output redirections detected. Cannot continue.");
                return -1;
            }
            redirectFlag = 1;
            outFlag = 1;
            ampFlag = 1;
            oRedirect = 1;
        }
    }
    clearFlag = 0;
    if (ampFlag == 1)
    {
        argptr[ptridx-1] = '\0';
        ptridx--;
    }
    if (blank == 1 && bang != 1) return -2;
    if (OUT == NULL && oRedirect == 1)
    {
        perror("No location specified for output redirection.");
        return -1;
    }
    if (IN == NULL && iRedirect == 1)
    {
        perror("No location specified for input redirection.");
        return -1;
    }
    return 0;
}



