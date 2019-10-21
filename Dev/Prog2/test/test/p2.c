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

int inFlag = 0;         //Flag for Input Redirection
int outFlag = 0;        //Flag for Output Redirection
int ampFlag= 0;         //Flag for Background Process 
int idx = 0;            //Index flag for Argument Char array
int ptridx = 0;         //Pointer index for pointer array
int IORedirect = 0;     //Flag for general I/O Redirection
int isEmpty = 0;        //Flag to detect command line argument
int clearFlag = 0;      //Flag to specifiy if 
int blank = 1;          
int cmdHandledFlag = 0; //Flag to see if command line argument has been addressed
int cmdFlag = 0;        //Flag to check command line arguments method
int redirectFlag = 0;   //Flag for redirection method
int multiArg = 0;       //check if multi argument command ex: 'echo -e -n'
int lastCmdCD = 0;      //Flag if last command was a cd
int bangFlag = 0;       //Flag for !! command


//Copy of Variables used for !! command
int cinFlag = 0;
int coutFlag = 0;
int campFlag= 0;
int cidx = 0;
int cptridx = 0;
int *cIN = NULL;
int *cOUT = NULL;
int credirectFlag = 0;
int cmultiArg = 0;


//IN and OUT redirection pointers for names of file
int *OUT = NULL;
int *IN = NULL;

//Arrays to store arguments and commands
char *argptr[MAXITEM];
char parameters[MAXITEM][STORAGE];
char s[STORAGE];

//Copy arrays for above
char cmdcpy[STORAGE];
char *cpyptr[MAXITEM];
char cpypara[MAXITEM][STORAGE];

//Helper/Debug method
static void printFlags()
{
    printf("< = %d\n", inFlag);
    printf("> = %d\n", outFlag);
    printf("& = %d\n", ampFlag);
}

//Method to clear flags
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
    multiArg = 0;
}

//Void method to handle sig interupt command from user
void myhandler(int signum)
{
    //return;
}

//Method to copy flags, arguments, and pointers before wiping (a copy)
void copyOver()
{
    int i;
    int j;

    cinFlag = inFlag;
    coutFlag = outFlag;
    campFlag= ampFlag;
    cidx = idx;
    cptridx = ptridx;
    cIN = IN;
    cOUT = OUT;
    credirectFlag = redirectFlag;
    cmultiArg = multiArg;

    for (i = 0; i < idx; i++) {
        strncpy(cpypara[i], parameters[i], sizeof(parameters[i]));
    }

    for (j = 0; j < ptridx; j++) {
        cpyptr[j] = argptr[j];
    }

}

//Method to restore flags, arguments and pointers back if !! was detected
void copyBack()
{
    int i;
    int j;

    inFlag = cinFlag;
    outFlag = coutFlag;
    ampFlag= campFlag;
    idx = cidx;
    ptridx = cptridx;
    IN = cIN;
    OUT = cOUT;
    redirectFlag = credirectFlag;
    multiArg = cmultiArg; 

    for (i = 0; i < idx; i++) {
        strncpy(parameters[i], cpypara[i], sizeof(cpypara[i]));
    }

    for (j = 0; j < ptridx; j++) {
        argptr[j] = cpyptr[j];
    }

}

/* P2 is an implementation of a basic C shell.
It has the ability to execute programs as child processes, use I/O redirection,
change directories, and repeat commands. Many of these features work at a basic level.
There are a few problems apparent though. More specification in gradernotes.
*/
int main (int argc, char *argv[])
{
    char *prompt = "%1% ";
    int c;
    int cmdArg;
    int cmdDup;
    pid_t child_pid, parent_pid, tpid;
    int child_status;
    DIR * dir;

//set the Process Group ID so sigterm will not kill the autograder/exits gracefully
    setpgid(0, 0);
    (void) signal(SIGTERM, myhandler);

//if there is more than 2 argc words throw an error
    if (argc > 2) 
    {
        perror("Too many command line arguments.");
        exit(9);
    }

//if all is good and there are excatly 2 arguments. Open file and make it new stdin
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
        //copy the flags and arrays before wiping memorr
        copyOver();

        //wipe
        clearFlags();
        memset(argptr, 0, MAXITEM * sizeof *argptr);
        memset(parameters, 0, MAXITEM * sizeof *parameters);

        if (cmdHandledFlag == 1){
            blank = 1;
            if (isEmpty != 1) 
            {
                printf("%s", prompt);
            }
            c = parse(s, argptr, parameters);
        }

        if (c == -11)
        {
            //Copy flags and pointer/arguments back into original
            copyBack();
        }


//Used extensively for debugging/testing
        //checkPar(argptr, parameters);
        //printf("idx %d\n", idx);
        //Testing purposes
        //printf("%d ----\n", c);
        //printFlags();
        //printf("%d -- IN\n", IN);
        //printf("%d -- OUT: \n", OUT);
        //printf("arg%d arg --- %d\n", argc, argptr[1]);
        //printf("%s\n", argptr);
//-------------------------------------------------

        if (c == -1) break;

        //Empty Line recieved continue through loop
        else if (c == -2 || c == -255) 
        {
            fflush(stdout);
            continue;
        }

        //CD was detected as command to execute. We must implement ourselves
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
                    //attempt to open directory to see if it exists
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
                //if cd was entered without argument change to "home" directory
                chdir(getenv("HOME"));
            }
        }

        //if done is the command exit
        else if ((strcmp(parameters[0],"done")) == 0)
        {
            break;
        }

        else {
            //execute command given with arguments and flags set
            execcmd();
        }
        cmdHandledFlag = 1;

    }
    killpg(getpgrp(), SIGTERM);
    printf("p2 terminated.\n"); //prtintf comes after killpg
    exit(0);
}

/* Method used to check the Parsing of Parse().
Check to see if everything is loaded in the propper sequence.
*/
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

/* Method to execute the command given. The void function will first declare all needed
variables. There are certain edge cases that should be handled. Redirection function is called
from within here to set up correct I/O. Finally parent and child code in cases of 
a background flag and it's absense.
*/
void execcmd() {
    pid_t child_pid, tpid;
    int child_status;
    int redirection_return;
    int dev;
    char *cmd2[] = { "tr", "a-z", "A-Z", (char*) NULL };

    //flush out both standar out and error
    fflush(stdout);
    fflush(stderr);

    //if command was entered with i/o like ex: < input1 > out echo
    //Does not work with tr in test for some reason.
    if ((ptridx != 0) && ((strcmp(parameters[0],"<") == 0) || (strcmp(parameters[0],">") == 0)) )
    {
        strncpy(parameters[0],argptr[0], sizeof(argptr[0]));
    }

    //if empty return
    if (isEmpty == 1) return;

    //Begin fork of child proccess. saving PID 
    if ((child_pid=fork()) == -1) 
    {
        perror("Cannot Fork.");
        return;
    }

    else if(child_pid == 0) {
        //set up redirection in child.
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

/*  Function to set up input and output redirection if specified by parse.
IN and OUT are pointers to the arguments holding the name of files. These files
are opened if they exist and using dup2() we redirect STDIN and STDOUT
*/
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

        if(dup2(ifd, 0) < 0)
        {
            perror("Dup2() has failed to redirect input.");
            exit(-5);
        }
        //Load file contents into arrays
        parseInputFile();
        close(ifd);
    }
}

/*  Specific Parsing for input file. This is a design decision because I noticed
having a file that contains a statement such as ex "echo < input1" throws errors
due to the < metacharacter. By ignoring those characters all content is loaded as arguments.
I assumed this is exactly what the user wants such as Echo "Echo < input1".
*/
int parseInputFile()
{
   int c;

    while((c=getword(s)) != 0)
    {
        if (c == 0 && idx == 0) return -2;

        if((c == -1) && (idx == 0)) 
        {
            return -1;
        }
        if (c==-1) break;

        strcpy(parameters[idx++],s);
        argptr[ptridx++] = parameters[idx-1];
    }
    return 0;
}

/* Function to check if file exists
*/
int file_exists(const char *file_name)
{
    return access(file_name, F_OK);
}

/* Parse function that takes advantage of previious getword() c file.
As words are written to char s we can check based on return values if they
should be added to the array. If they are meta characters they are added to the 
commands array but not the arguments array.
*/
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

        //if bang is encountered return to main
        if ((strcmp(s,"!!") == 0) && idx == 0)
        {
            bangFlag = 1;
            return -11;
        }

        strcpy(parameters[idx++],s);
        ampFlag = 0;

        //check if output metacharacter was detected
        if (strcmp("<", s) == 0) 
        {
            //if there are mutliple characters throw an error
            if (inFlag == 1) {
                perror("Multiple input redirections detected. Cannot continue.");
                return -255;
            }
            redirectFlag = 1;
            inFlag = 1;
            iRedirect = 1;
        }
        //check if input metacharacter was detected
        if (strcmp(">", s) == 0)
        {
            //if there are mutliple characters throw an error
            if (outFlag == 1) {
                perror("Multiple output redirections detected. Cannot continue.");
                return -255;
            }
            redirectFlag = 1;
            outFlag = 1;
            oRedirect = 1;
        }
        //check if background metacharacter was detected
        if (strcmp("&", s) == 0) ampFlag = 1;

        //check if background-output metacharacter was detected
        if (strcmp(">&", s) == 0)
        {
            //if there are mutliple characters throw an error
            if (outFlag == 1) {
                perror("Multiple output redirections detected. Cannot continue.");
                return -255;
            }
            redirectFlag = 1;
            outFlag = 1;
            ampFlag = 1;
            oRedirect = 1;
        }

        //if word is not a metacharacter and not the & symbol add it to arguments
        //We add amppersand becuase it only has an effect if it is the last character in a command
        if (c != 2048 || (strcmp(s,"&") == 0) )
        {
            //if output redirection character found make the next found argument its location
            if (OUT == NULL && oRedirect == 1 && (strcmp(s,"&") != 0))
            {
                OUT = parameters[idx-1];
            }
            //if input redirection character found make the next found argument its location
            else if (IN == NULL && iRedirect == 1 && (strcmp(s,"&") != 0))
            {
                IN = parameters[idx-1];
            }
            else {
                //if it made it this far it should be on the argument array
                argptr[ptridx++] = parameters[idx-1];
            }

        }
    }
    clearFlag = 0;

    //check if background process flag was triggered in Parse
    if (ampFlag == 1)
    {
        argptr[ptridx-1] = '\0';
        //Decrement ptridx and removes it from argument array
        ptridx--;
    }
    //check if output redirection was triggered but no file given
    if (OUT == NULL && oRedirect == 1)
    {
        perror("No location specified for output redirection.");
        return -255;
    }
    //check if input redirection was triggered but no file given
    if (IN == NULL && iRedirect == 1)
    {
        perror("No location specified for input redirection.");
        return -255;
    }
    return 0;
}

