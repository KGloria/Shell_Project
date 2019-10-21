/*  Name: Kyle Gloria
    Class: CS570 Operating Systems
    Instructor: John Carroll
    Term: Fall 2019 
    School: San Diego State University
    Due Date: September 4, 2019
*/

#include <stdio.h>
#include <string.h>
#include "getword.h"

/* getword() function will read each word of the input string until EOF is
    enconuntered. 
        - '\n' If a newline is encountered it will output n=0 and an empty
            string. 
        - 'EOF' will return n=-1 and an empty string as well. 
            If 'done' is read from the input stream n=-1 and the word done will be output.
*/

int getword(char *w) 
{
    //Declaration of size, char holder integer , and stopcode
    int size = 0;
    int iochar;
    char stopcode[] = "done";

    while ((iochar=getchar()) != EOF) {

        //Handle leading 0's 
        while (iochar == ' ' && size == 0) {
            iochar=getchar();
            if (iochar == '\n') {
                w[size] = '\0';
                return 0;
            }
        } 

        //After leading 0's are handled check to see if Newline is encountered 
        if (iochar == '\n' && size == 0) { 
            w[size] = '\0';
            return 0; 
        }

        if (iochar == '\n' && size != 0) { 
            ungetc(iochar, stdin);
            w[size] = '\0';
            if (strcmp(w, stopcode) == 0) {
                return -1;
            }
            return size;
        }

        //Check if a space is encountered
        if (iochar == ' ') {
            w[size] = '\0';
            if (strcmp(w, stopcode) == 0) {
                return -1;
            }
            return size;
        }

        //Incremeent the size of the word
        if (iochar != ' ' || iochar != '\0' || iochar != '\n') {
            w[size] = iochar;
            size++;
        }
    }
    //If size was not empty return the last word
    w[size] = '\0';
    if (size != 0) {
        return size;
    }
    return -1;
}