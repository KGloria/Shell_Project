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

int fd;
int fd2;
int fd3;
int fd4;

double total;
int mid, numRow;
sem_t *pmutx; /* semaphore guarding access to shared data */
int count; /* local copy of counter */
int row;

int midReached;
int printcnt;

char semaphoreMutx[SEMNAMESIZE];


void handle_sigint(int sig) 
{ 
	close(fd);
	unlink("countfile");
	close(fd2);
	unlink("rowfile");
	sem_close(pmutx);
	sem_unlink(semaphoreMutx); 
} 

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
  
void initStudentStuff(void){
  	sprintf(semaphoreMutx,"/%s%ldmutx",COURSEID,(long)getuid());
  	signal(SIGINT, handle_sigint);
  	signal(SIGTSTP, handle_sigint);

	total = nrRobots * quota;
	mid = (total/2);	

	int temp = 0;
	int i = 0;
	int j = 0;

	while (temp < (int)total) {
		i++;
		if (temp < total) {
			temp = temp + i;
		}
		else if (temp > mid) {
			j++;
			temp = temp + (i-j);
		}
		else if (temp == (int)total) {
			i++;
		}
	}
	numRow = i;  

	if((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR, 0)) != SEM_FAILED){
		//sleep(1);
		//Initialize the file
		CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		CHK(fd2 = open("rowfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		CHK(fd3 = open("printfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		CHK(fd4 = open("midfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		count = 0;
		CHK(lseek(fd, 0, SEEK_SET));
		assert(sizeof(count) == write(fd, &count, sizeof(count))); //write count to file
		row = 1;
		midReached = 0;
		printcnt = 0;

		CHK(lseek(fd2, 0, SEEK_SET));
		assert(sizeof(row) == write(fd2, &row, sizeof(row)));  //write row to file

		CHK(lseek(fd3, 0, SEEK_SET));
		assert(sizeof(row) == write(fd3, &printcnt, sizeof(printcnt)));  //write printcnt to file

		CHK(lseek(fd4, 0, SEEK_SET));
		assert(sizeof(row) == write(fd4, &midReached, sizeof(midReached)));  //write midcnt to file

		CHK(sem_post(pmutx)); //files are now initialized
	}
	else{
		//sleep(rand() % 2);
		CHK((int)(pmutx = sem_open(semaphoreMutx, O_RDWR, 1)));
		CHK(sem_wait(pmutx));
		CHK(fd = open("countfile", O_RDWR));
		CHK(fd2 = open("rowfile", O_RDWR));

		CHK(fd3 = open("printfile", O_RDWR));
		CHK(fd4 = open("midfile", O_RDWR));

		CHK(sem_post(pmutx));
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

	CHK(lseek(fd3, 0, SEEK_SET));
	assert(sizeof(printcnt) == read(fd3, &printcnt, sizeof(printcnt)));

	CHK(lseek(fd4, 0, SEEK_SET));
	assert(sizeof(midReached) == read(fd4, &midReached, sizeof(midReached)));

	count++;

	if(count == ((nrRobots * quota))){
		printeger(n);
		printf("F\n");
		printf("--mid %d\n", mid);
		printf("--numRow %d\n", numRow);
		printf("Flag mid-%d\n", midReached);
		fflush(stdout);
		CHK(close(fd));
		CHK(unlink("countfile"));
		CHK(close(fd2));
		CHK(unlink("rowfile"));

		CHK(close(fd3));
		CHK(unlink("midfile"));

		CHK(close(fd4));
		CHK(unlink("printfile"));

		CHK(sem_close(pmutx));
		CHK(sem_unlink(semaphoreMutx));
	}
	else{
		int temp = count;
		int i;

		for (i=0; i < row; i++) {
			count = count - i;
		}

		printeger(n);
		//printf("|c-%d|", count);
		fflush(stdout);

		if (row == count) {
			row++;
			if (row == mid/2) {
				midReached = 1;
			}
			printf("N\n");
			fflush(stdout);
		}

		count = temp;
		CHK(lseek(fd,0,SEEK_SET));
		assert(sizeof(count) == write(fd, &count, sizeof(count)));

		CHK(lseek(fd2,0,SEEK_SET));
		assert(sizeof(row) == write(fd2, &row, sizeof(row)));

		CHK(lseek(fd4,0,SEEK_SET));
		assert(sizeof(midReached) == write(fd4, &midReached, sizeof(midReached)));

		/* end critical section */
		CHK(sem_post(pmutx)); /* release critical section */
	}
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */



