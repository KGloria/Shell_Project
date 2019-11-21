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
#define TRUE 1

int numCmds = 1;

int inFlag = 0;
int outFlag = 0;
int ampFlag= 0;
int idx = 0;
int ptridx = 0;

int IORedirect = 0;

int isEmpty = 0;

int clearFlag = 0;
int blank = 1;
int lastCmdCD = 0;

int cmdHandledFlag = 0;
int cmdFlag = 0;
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
    //return;
}

int main (int argc, char *argv[])
{
    char *prompt = "%1% ";
    int c;
    int cmdArg;
    int cmdDup;

    pid_t child_pid, parent_pid, tpid;
    int child_status;

    DIR * dir;

    setpgid(0, 0);
    (void) signal(SIGTERM, myhandler);

    if (argc > 2) 
    {
        perror("Too many command line arguments.");
        exit(9);
    }
    
    if (argc > 1 && cmdHandledFlag == 0) {
        cmdArg = open(argv[1], O_RDONLY);
        cmdDup = dup2(cmdArg, fileno(stdin));
        close(cmdArg);
    }
    else {
        cmdHandledFlag = 1;
    }

    while(TRUE)
    {    
        clearFlags();
        memset(argptr, 0, MAXITEM * sizeof *argptr);
        memset(parameters, 0, MAXITEM * sizeof *parameters);

        if (cmdHandledFlag == 1){
            blank = 1;
            if (isEmpty != 1) 
            {
                printf("%%");
                printf("%d", numCmds);
                printf("%%");

            }
            c = parse(s, argptr, parameters);
            if (argptr[0] != NULL)
                numCmds++;
        }

        
        //checkPar(argptr, parameters);
        //printf("idx %d\n", idx);
        //Testing purposes
        //printf("%d ----\n", c);
        //printFlags();

        //printf("%d -- IN\n", IN);
        //printf("%d -- OUT: \n", OUT);
        //printf("arg%d arg --- %d\n", argc, argptr[1]);
        //printf("%s\n", argptr);

        if (c == -1) break;

        else if (c == -2 || c == -255) 
        {
            fflush(stdout);
            continue;
        }

        else if ((strcmp(parameters[0],"cd")) == 0)
        {
            lastCmdCD = 1;
            if (ptridx > 1){
                // Check if directory exists
                
                if(ptridx == 3 && strcmp(parameters[2],"&") != 0) 
                {
                    perror("Too many arguments to CD.");
                }
                if (ptridx > 3) 
                {
                    perror("Too many arguments to CD.");
                }
                else {
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
            }
            else 
            {
                    chdir(getenv("HOME"));
            }
        }

        else if ((strcmp(parameters[0],"done")) == 0)
        {
            break;
        }
        else {
            execcmd();
            clearFlags();
        }
        cmdHandledFlag = 1;

    }

    killpg(getpgrp(), SIGTERM);
    printf("p2 terminated.\n"); //prtintf comes after killpg
    exit(0);
}


void checkPar(char *argptr[MAXITEM], char parameters[][STORAGE])
{
    int i = 0;
    int j = 0;
        for (i=0; i < idx; i++)
    {
        printf("addess:%d parameter -- %s\n", &parameters[i], parameters[i]);
    }

    for (j=0; j < ptridx; j++)
    {
        printf("*ptr address >:%d arg ptr -- %s\n", *argptr[j],argptr[j]);
    }
}


void execcmd() {
    pid_t child_pid, tpid;
    int child_status;
    int redirection_return;
    int dev;

    fflush(stdout);
    fflush(stderr);

    if (isEmpty == 1) return;

    if ((child_pid=fork()) == -1) 
    {
        perror("Cannot Fork.");
        return;
    }

    else if(child_pid == 0) {

        if (redirectFlag)
        {
            redirection_return = redirectSetUp();
        }

        if (redirection_return == -1) return;

        if(IN == NULL && ampFlag != 0)
        {        
            dev = open("/dev/null",STDIN_FILENO);
            dup2(dev, STDIN_FILENO);
            close(dev);
        }

        /* This is done by the child process. */     

        execvp(parameters[0], argptr);
        /* If execvp returns, it must have failed. */
        if (idx != 0 || isEmpty == 1) printf("Unknown command\n");
        else isEmpty == 1;
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
        else if (ampFlag) 
        {
            printf("%s [", parameters[0]);
            printf("%d]\n", child_pid);
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
    int ofd = 0;
    int ifd = 0;

    if(OUT != NULL)
    {
        if (file_exists(OUT) == 0)
        {
            perror("File exists");
            return -1;
        }
        ofd = open(OUT,O_RDWR | O_CREAT | O_TRUNC, S_IRUSR, S_IWUSR);

        if (ofd < 0) 
        {
            perror("Existing file. Cannot overwrite.");
            exit(-2);
        }

        if(dup2(ofd, STDOUT_FILENO) < 0)
        {
            perror("Dup2() has failed to redirect output.");
            exit(-4);
        }

        if(dup2(ofd, STDERR_FILENO) < 0)
        {
            perror("Dup2() has failed to redirect output.");
            exit(-4);
        }
        close(ofd);
    }

    if(IN != NULL)
    {
        ifd = open(IN, O_RDONLY);
        if (ifd < 0) 
        {
            perror("Cannot read input.");
            exit(-3);
        }

        if(dup2(ifd, STDIN_FILENO) < 0)
        {
            perror("Dup2() has failed to redirect input.");
            exit(-5);
        }
        close(ifd);
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
    int setio = 0;

    while((c=getword(s)) != 0)
    {
        if (c == 0 && idx == 0) return -2;

        if((c == -1) && (idx == 0)) 
        {
            clearFlag = 0;
            return -1;
        }
        if (c==-1) break;
        strcpy(parameters[idx++],s);
        ampFlag = 0;

        if (strcmp("<", s) == 0) 
        {
            if (inFlag == 1) {
                perror("Multiple input redirections detected. Cannot continue.");
                return -255;
            }
            redirectFlag = 1;
            inFlag = 1;
            iRedirect = 1;
        }
        if (strcmp(">", s) == 0)
        {
            if (outFlag == 1) {
                perror("Multiple output redirections detected. Cannot continue.");
                return -255;
            }
            redirectFlag = 1;
            outFlag = 1;
            oRedirect = 1;
        }
        if (strcmp("&", s) == 0) ampFlag = 1;
        if (strcmp(">&", s) == 0)
        {
            printf("redirect& \n");
            if (outFlag == 1) {
                perror("Multiple output redirections detected. Cannot continue.");
                return -255;
            }
            redirectFlag = 1;
            outFlag = 1;
            ampFlag = 1;
            oRedirect = 1;
        }

        if (c != 2048 || (strcmp(s,"&") == 0) )
        {
            if (OUT == NULL && oRedirect == 1 && (strcmp(s,"&") != 0))
            {
                OUT = parameters[idx-1];
            }
            else if (IN == NULL && iRedirect == 1 && (strcmp(s,"&") != 0))
            {
                IN = parameters[idx-1];
            }
            else {
                argptr[ptridx++] = parameters[idx-1];
            }

        }
    }
    clearFlag = 0;
    if (ampFlag == 1)
    {
        argptr[ptridx-1] = '\0';
        ptridx--;
    }
    if (OUT == NULL && oRedirect == 1)
    {
        perror("No location specified for output redirection.");
        return -255;
    }
    if (IN == NULL && iRedirect == 1)
    {
        perror("No location specified for input redirection.");
        return -255;
    }
    return 0;
}

