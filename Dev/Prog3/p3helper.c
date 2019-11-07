/* p3helper.c
   Kyle Gloria 
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

extern int nrRobots;
extern int quota;
extern int seed;

/* You may put declarations/definitions here.
   In particular, you will probably want access to information
   about the job (for details see the assignment and the documentation
   in p3robot.c):
     */

int fd, fd2, fd3, fd4; /* File Descriptors (countfile, rowfile, printfile, midfile */
int count; /* local copy of counter */
int row; /* Variable to track total number of rows*/
int midReached; /* Variable to track if the mid widget is passed. can only be 0 or 1.*/
int printcnt; /* Variable to keep track of */
int mid, numRow; /* Variable to keep track of middle widget. numRows is unused */
double total; /* Variable to get total number of widgets as double type*/

sem_t *pmutx; /* semaphore guarding access to shared data */
char semaphoreMutx[SEMNAMESIZE];

/* Function to handle SIG_STOP and SIG_INT signals.
When these signals are detacted (the program is shut down forcefully) 
all file descriptors, files and mutexes should be uninked or closed.
*/
void handle_sigint(int sig) 
{ 
	close(fd);
	unlink("countfile");
	close(fd2);
	unlink("rowfile");
	close(fd3);
	unlink("printfile");
	close(fd4);
	unlink("midfile");	
	sem_close(pmutx);
	sem_unlink(semaphoreMutx); 
} 

/* General documentation for the following functions is in p3.h
   Here you supply the code, and internal documentation:
   */
  
void initStudentStuff(void){

  	sprintf(semaphoreMutx,"/%s%ldmutx", COURSEID, (long)getuid());

	//sig_handlers
  	signal(SIGINT, handle_sigint);
  	signal(SIGTSTP, handle_sigint);

	//variables to find total and middle widget numbers
	total = nrRobots * quota;
	mid = (total/2);	

//Statement for testing purposes
/*
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
*/

	//If this is the first process to open the semaphore then initialize all variables and files.
	//This is done by opening the mutex as locked so no other process may access the critical seciton until
	//All files are created and initialized properly.
	if((pmutx = sem_open(semaphoreMutx, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR, 0)) != SEM_FAILED){
		//sleep(1);

		count = 0;
		row = 1;
		midReached = 0;
		printcnt = 0;

		CHK(fd = open("countfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		CHK(fd2 = open("rowfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		CHK(fd3 = open("printfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));
		CHK(fd4 = open("midfile", O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR));


		/*Set all file descriptors to beginning of file and write the initial values to each*/
		CHK(lseek(fd, 0, SEEK_SET));
		assert(sizeof(count) == write(fd, &count, sizeof(count))); 

		CHK(lseek(fd2, 0, SEEK_SET));
		assert(sizeof(row) == write(fd2, &row, sizeof(row)));  

		CHK(lseek(fd3, 0, SEEK_SET));
		assert(sizeof(row) == write(fd3, &printcnt, sizeof(printcnt)));  

		CHK(lseek(fd4, 0, SEEK_SET));
		assert(sizeof(row) == write(fd4, &midReached, sizeof(midReached)));  

		//Increment the Semaphore to allow other processes to access the critical section
		CHK(sem_post(pmutx)); 
	}
	//All other processes will go here to open the semaphore as unlocked and wait for access to the critical section
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

	/* Request access and wait for Critical Section */
	CHK(sem_wait(pmutx));

	/* Start of Critical Section */

	//Update value of count from countfile
	CHK(lseek(fd, 0, SEEK_SET));
	assert(sizeof(count) == read(fd, &count, sizeof(count)));

	//Update value of row from rowfile
	CHK(lseek(fd2, 0, SEEK_SET));
	assert(sizeof(row) == read(fd2, &row, sizeof(row)));

	//Update value of printcnt from printfile
	CHK(lseek(fd3, 0, SEEK_SET));
	assert(sizeof(printcnt) == read(fd3, &printcnt, sizeof(printcnt)));

	//Update value of midReached from midfile
	CHK(lseek(fd4, 0, SEEK_SET));
	assert(sizeof(midReached) == read(fd4, &midReached, sizeof(midReached)));

	//Once assertion of count value has been completed we can increment count
	count++;

	//if total number of widgets has been reached
	if(count == ((nrRobots * quota))){
		//place the last widget and print F to signify end 
		printeger(n);
		printf("F\n");
		fflush(stdout);

		//Unlink and close all files
		CHK(close(fd));
		CHK(unlink("countfile"));
		CHK(close(fd2));
		CHK(unlink("rowfile"));
		CHK(close(fd3));
		CHK(unlink("midfile"));
		CHK(close(fd4));
		CHK(unlink("printfile"));

		//Unlink and close semaphore
		CHK(sem_close(pmutx));
		CHK(sem_unlink(semaphoreMutx));
	}
	else{
		//Increment print count (this variable is counting the current number of widgets in a row)
		printcnt++;
		printeger(n);
		fflush(stdout);

		//Row variable specifies how many widgets in each row. When printcnt is equal print N to end row
		if (row == printcnt) {
			if (midReached == 0) {	
				row++; 
			}
			printcnt = 0;
			printf("N\n");
			fflush(stdout);

			//If the middle widget has been passed set middle reached flag.
			if (count > mid) {
				midReached = 1;
			}
		}
		//if middle widget has been reached we must start decrementing row count. (This is row - 2 because we previously increment)
		if (midReached == 1) {
			row = row - 2;
			midReached = 0; //Reset middle reached or else we will decrement for each widget in the row
		}
		
		//Write to each file after assertion to update count/flags
		CHK(lseek(fd,0,SEEK_SET));
		assert(sizeof(count) == write(fd, &count, sizeof(count)));

		CHK(lseek(fd2,0,SEEK_SET));
		assert(sizeof(row) == write(fd2, &row, sizeof(row)));

		CHK(lseek(fd3,0,SEEK_SET));
		assert(sizeof(printcnt) == write(fd3, &printcnt, sizeof(printcnt)));

		CHK(lseek(fd4,0,SEEK_SET));
		assert(sizeof(midReached) == write(fd4, &midReached, sizeof(midReached)));


		/* End of the critical section */
		/* Release of the critical section */
		CHK(sem_post(pmutx)); 
	}
}

/* If you feel the need to create any additional functions, please
   write them below here, with appropriate documentation:
   */



