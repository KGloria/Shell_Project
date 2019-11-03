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

int fd, fd2, row, count, mid;
double total;
sem_t *pmutx; /* semaphore guarding access to shared data */
char semaphoreMutx[SEMNAMESIZE];

void handle_sigint(int sig) 
{ 
    CHK(close(fd));
    CHK(unlink("countfile"));
    CHK(close(fd2));
    CHK(unlink("rowfile"));
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
  total = nrRobots * quota;
  mid = ceil(total/2);
  fflush(stdout);
  if((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,1)) == SEM_FAILED){
 	
	CHK((int)(pmutx = sem_open(semaphoreMutx,O_RDWR)));
	CHK(fd = open("countfile", O_RDWR));
	CHK(fd2 = open("rowfile", O_RDWR));
  }
  else{
 	  //Initialize the file
	
	  CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
	  CHK(fd2 = open("rowfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
  	count = 0;
	  CHK(sem_wait(pmutx)); //wait for files to be initialized
  	CHK(lseek(fd,0,SEEK_SET));
  	assert(sizeof(count) == write(fd,&count,sizeof(count))); //write count to file
  	row = 1;
  	CHK(lseek(fd2,0,SEEK_SET));
  	assert(sizeof(row) == write(fd2,&row,sizeof(row)));  //write row to file
	  CHK(sem_post(pmutx)); //files are now initialized
	
  }
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
        /* request access to critical section */
    CHK(sem_wait(pmutx));
    /* begin critical section -- read count, increment count, write count */
    CHK(lseek(fd, 0, SEEK_SET));
    assert(sizeof(count) == read(fd, &count, sizeof(count)));
    //rowfile set
    CHK(lseek(fd2, 0, SEEK_SET));
    assert(sizeof(row) == read(fd2, &row, sizeof(row)));
    count++;
    int tmp = count;
    int temp = row;
    printf("r--%d ", row);
    //printf("c--%d ", count);
    if(count == ((nrRobots * quota))){
      printeger(n);
      printf("F\n");
      printf("mid -  %d\n", mid);
      fflush(stdout);
      CHK(close(fd));
      CHK(unlink("countfile"));
      CHK(close(fd2));
      CHK(unlink("rowfile"));
      CHK(sem_close(pmutx));
      CHK(sem_unlink(semaphoreMutx));
    }
    else{
      int i, j;
      for(i=1; i<row ; i++){
        count=count-i;
      }

  
      printf("c--%d ", count);
      if(count == row){
        printeger(n);
        if (tmp > mid) {
          printf("mid");
        }
        printf("N\n");
        fflush(stdout);
        row++;
      }
    else{
      printeger(n);
      fflush(stdout);
    }
    count=tmp;
    CHK(lseek(fd,0,SEEK_SET));
    assert(sizeof(count) == write(fd, &count, sizeof(count)));
    CHK(lseek(fd2,0,SEEK_SET));
    assert(sizeof(row) == write(fd2, &row, sizeof(row)));
    /* end critical section */
    CHK(sem_post(pmutx)); /* release critical section */
    }
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */



