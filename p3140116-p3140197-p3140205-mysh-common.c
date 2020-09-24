#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "p3140116-p3140197-p3140205-mysh.h"


char *read_line(void) {
  int bufsize = 255;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  while (1) {
    // Read a character
    c = getchar();
	if(c == '\n') { 
        return buffer;
    } else if (position < bufsize) {
      buffer[position] = c;
      position ++;
    } else {
		printf("\nLine can only accept %d characters. You just reached them.\n", bufsize);
		return buffer;
	}
  }
}

char *concat(char *s1,char *s2) { 
	char *result = malloc(strlen(s1) + strlen(s2) +1);
	strcpy(result, s1);
	strcat(result, s2);
	return result;
 }




