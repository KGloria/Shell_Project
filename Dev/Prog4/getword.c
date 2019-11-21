/*  Name: Kyle Gloria
    Class: CS570 Operating Systems
    Instructor: John Carroll
    Term: Fall 2019 
    School: San Diego State University
    Due Date: September 16, 2019
*/

#include <stdio.h>
#include <string.h>
#include "getword.h"
int MAX_SIZE = 255;

/* The function CheckChar() checks if a meta characeter is encountered. This function does not modify the stdin and
uses ungetc in a reversed order to place characters back on stdin. If a meta character is encountered an integer greater
than 0 will be returned.
*/
static int CheckChar(int iochar) {
    int metaCharSize = 0;
    //A switch checking iochar is used to determine the meta character. If the metacahracter is > keep using getchar()
    // to enact the "greedy" read in of a line (looking for the longest meta character). After the meta character
    // is determined place the meta characters back onto stdin and return the integer to identify the meta character.
    switch (iochar)
    {
        case '<': 
            ungetc('<', stdin);
            return 1;

        case '>':
            iochar = getchar();
            if (iochar == '&') {
                ungetc('&', stdin);
                ungetc('>', stdin);
                return 7;
            }
            else if (iochar == '>') {
                iochar = getchar();
                if (iochar == '&') {
                    ungetc('&', stdin);
                    ungetc('>', stdin);
                    ungetc('>', stdin);
                    return 8;
                }
                else {
                    ungetc(iochar, stdin);
                    ungetc('>', stdin);
                    ungetc('>', stdin);
                    return 6;
                }
            }
            else {
                ungetc(iochar, stdin);
                ungetc('>', stdin);
                return 2;
            }

        case '|': 
            ungetc('|', stdin);
            return 3;

        case '#': 
            ungetc('#', stdin);
            return 4;
        
        case '&': 
            ungetc('&', stdin);
            return 5;     

    }
    return 0;
}

/* The function moveChar() takes in a pointer to the character array and an integer containing the meta char type.
Using a case statemnt the function checks to the type of the meta char and uses getchar to place it on the character
array. When it is done it ends with the null terminator and returns the size of the meta character passed to it, else it 
returns 0.
*/
static int moveChar(char *w, int metaCharType) {
    int metaCharSize = 0;
    //switch used to place the correct meta character on char array
    switch (metaCharType)
    {
        case 1: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 1;

        case 2:
            w[metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 1;

        case 3: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 1;

        case 4: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 1;

        case 5: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 1;   

        case 6: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 2;

        case 7: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 2;

        case 8: 
            w[metaCharSize] = getchar();
            w[++metaCharSize] = getchar();
            w[++metaCharSize] = getchar();
            w[++metaCharSize] = '\0';
            return 3;       

    }
    return 0;
}

/*The function returnMeta() takes in a integer value that represents a meta character type.
The purpose of this funciton is more so for future assignments in order to keep track of which meta
character is read in.
*/
static int returnMeta(int metaCharType) {
    switch (metaCharType)
    {
        case 1:
            return 1;
        case 2:
            return 1;        
        case 3:
            return 1;
        case 4:
            return 1;
        case 5:
            return 1;
        case 6:
            return 2;
        case 7:
            return 2;
        case 8:
            return 3;
    }
    return 0;
}

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
    int metaCharType = 0;
    int iochar;
    char stopcode[] = "done";
    int length = strlen(w);

    //getchar() is used to peek ahead at the next char. While ungetc() is used to place a char back on stdin if 
    //is not the char we wanted
    while ((iochar=getchar()) != EOF) {

        //check the size of the current word at each start of the while loop
        if (size >= 254) {
            //if the size of the word is greater than 254 return the current size and place the read char back onto stdin
            ungetc(iochar,stdin);
            return size;
        }

        //Handle leading 0's 
        while (iochar == ' ' && size == 0) {
            iochar=getchar();
            if (iochar == '\n') {
                w[size] = '\0';
                return size;
            }
        } 

        //After leading 0's are handled check to see if Newline is encountered 
        if (iochar == '\n' && size == 0) { 
            w[size] = '\0';
            return 0; 
        }
        else if (iochar == '\n' && size != 0) { 
            //place the newline character back on stdin using ungetc()
            ungetc(iochar, stdin);
            w[size] = '\0';
            if (strcmp(w, stopcode) == 0) {
                return -1;
            }
            return size;
        }

        //Check if the backslash character is encountered and size is zero
        else if(iochar == '\\' && size == 0) {
            iochar=getchar();

            //if a meta character is encountered place it on the character array and grab the next char from stdin.
            if (CheckChar(iochar)) {
                w[size++] = iochar;
                iochar=getchar();
            }

            //else if the character is not the null terminator or newline do the same.
            else if( (iochar != '\0' || iochar != '\n')) {
                w[size++] = iochar;
            }
        }

        //Check if the backslash character is encountered and size is not zero
        else if(iochar == '\\' && size != 0) {
            iochar=getchar();

            if (iochar == '\n') {
                //place the newline character back on stdin using ungetc()
                ungetc(iochar, stdin);
                w[size] = '\0';
                return size;
            }

            if( (iochar != '\0' || iochar != '\n')) {
                w[size++] = iochar;
            }
        }       

        //check if a meta character is encountered 
        else if ((metaCharType=CheckChar(iochar))) {
            //if size is not zero, place the null terminator on the char array and return the current size
            if (size != 0) {
                w[size] = '\0';
                return size;
            }
            //else place the metacharacter on the char array and return the size of the meta character
            else 
            {
                moveChar(w, metaCharType);
                return 2048;
            }
        }

        //terminate and return the current size if a space is encountered
        else if (iochar == ' ') {
            w[size] = '\0';
            return size;
        }

        //Incremeent the size of the word
        else if( (iochar != '\0' || iochar != '\n')) {
            w[size] = iochar;
            size++;
        }
    }
    //If size was not empty return the last word
    w[size] = '\0';

    //if the stopcode is encountered return -1
    if (strcmp(w, stopcode) == 0) {
        return -1;
    } 

    if (size != 0) {
        return size;
    }
    //if EOF is encountered return -1 
    return -1;
}
