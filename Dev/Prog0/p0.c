/* ~cs570/One/getword.h provides a good example of the level of documentation
 * appropriate for this course. This p0.c provides an outstandingly BAD example.
 */
#define STORAGE 256

#include "getword.h"
#include <stdio.h>

int main()

{
int c;
char s[STORAGE];

for(;;) {
	(void) printf("n=%d, s=[%s]\n", c = getword(s), s);
        if (c == -1) break;
        }
}
