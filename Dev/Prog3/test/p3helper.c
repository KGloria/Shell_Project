/* p3helper.c
   Program 3 assignment
   CS570
   SDSU
   Fall 2019
 *-----------------------------------------------------------------------
 * Name: p3helper
 * Purpose: 
 * Input parameters:  

 * Output parameters:
 
 * Other side effects: 

 * Routines called: 
 * References: The p3 assignment file.
 */
#include "p3.h"
#include <math.h>
#include <signal.h> 

#define SNAME "/sem"

/*------------YOU WRITE THIS IN p3helper.c:------------------------------
 * Name: initStudentStuff
 * Purpose: initialize any data structures (including semaphores) that
   the cooperating robots may need.
   (Such a generic function lump is not usually good style, but in
   this case is makes it easy for us to keep student and problem
   issues separate.)
 * Input parameters: None
 * Output parameters: None
 * Other side effects: ...
 * Routines called: ... No I/O is done, and there is no call to
   sleep, usleep, nanosleep.
 * References: The p3 assignment
 */


/* You may put declarations/definitions here.
   In particular, you will probably want access to information
   about the job (for details see the assignment and the documentation
   in p3robot.c):
     */
extern int nrRobots;
extern int quota;
extern int seed;

int fd, fd2, fd3, row, count, mid, numRow;
int middleNum;
double total;
sem_t *pmutx; /* semaphore guarding access to shared data */
char semaphoreMutx[SEMNAMESIZE];

void handle_sigint(int sig) 
{ 
  CHK(close(fd3));
  CHK(unlink("fuckingtest"));
  CHK(sem_close(pmutx));
  CHK(sem_unlink(semaphoreMutx)); 
} 

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
  
void initStudentStuff(void){
  sprintf(semaphoreMutx,"/%s%ldmutx", COURSEID, (long)getuid());
  signal(SIGINT, handle_sigint);
  signal(SIGTSTP, handle_sigint);
	CHK(fd3 = open("fuckingtest", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
  if((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR, 0)) == SEM_FAILED){
    CHK((int)(pmutx = sem_open(semaphoreMutx,O_RDWR)));
  }
  else{
 	  //Initialize the file
  }
	CHK(sem_post(pmutx)); //files are now initialized

}

/*------------YOU WRITE THIS IN p3helper.c:------------------------------
 * Name: placeWidget
 * Purpose: Place a widget "on the warehouse floor". Uses
   printeger() to print an integer, with new lines (and N or F)
   coordinated with others who are printing. The result of the
   coordination is that all the widgets (integers) get put out in the
   formatted way described in the p3 assignment.
 * Input parameters: an integer (the widget) to be printed
 * Output parameters: none.
 * Other side effects: none.
 * Routines called: ... printeger(), printf("N\n"), and printf("F\n")
   are the only output tools that placeWidget uses.
   fflush(stdout) is called after each call to printf.
   No calls are made to sleep, usleep, nanosleep.
 * References: p3 assignment.
 */

/* In this braindamaged version of placeWidget, the widget builders don't
   coordinate at all, and merely print a random pattern. You should replace
   this code with something that fully follows the p3 specification. */

void placeWidget(int n) {

    printeger(n);
    fflush(stdout);

}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */



