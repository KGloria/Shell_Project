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

static int CheckChar(int iochar) {
    int metaCharSize = 0;
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

static int moveChar(char *w, int iochar) {
    int metaCharSize = 0;
    switch (iochar)
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

static int returnMeta(int metaChar) {
    switch (metaChar)
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

    while ((iochar=getchar()) != EOF) {

        if (size >= 254) {
            ungetc(iochar,stdin);
            return size;
        }

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
        else if (iochar == '\n' && size != 0) { 
            ungetc(iochar, stdin);
            w[size] = '\0';
            if (strcmp(w, stopcode) == 0) {
                return -1;
            }
            return size;
        }

        else if(iochar == '\\' && size == 0) {
            iochar=getchar();

            if (CheckChar(iochar)) {
                w[size++] = iochar;
                iochar=getchar();
            }

            else if( (iochar != '\0' || iochar != '\n')) {
                w[size++] = iochar;
            }
        }

        else if(iochar == '\\' && size != 0) {
            iochar=getchar();

            if (iochar == '\n') {
                ungetc(iochar, stdin);
                w[size] = '\0';
                return size;
            }

            if( (iochar != '\0' || iochar != '\n')) {
                w[size++] = iochar;
            }
        }       

        else if ((metaCharType=CheckChar(iochar))) {
            if (size != 0) {
                w[size] = '\0';
                return size;
            }
            else return moveChar(w, metaCharType);
        }

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

    if (strcmp(w, stopcode) == 0) {
        return -1;
    } 

    if (size != 0) {
        return size;
    }
    return -1;
}