#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "p3140116-p3140197-p3140205-mysh.h"

int executeCommand(char *args[]);
char *read_line(void);
char *concat(char *s1,char *s2);

int main(int argc, char **argv)
{
  char *line;
  char **args;
  int status = 1;

  do {
    printf("mysh2> ");
    line = read_line();
    int i, argscount = 0;
    char *pch = strchr(line, ' ');
	while(pch != NULL){
		argscount++;
		pch = strchr(pch + 1, ' ');	
	}
	
    char *args[argscount +2];
    args[argscount + 1] = NULL;
    char * tok;
    tok = strtok(line, " ");
    i = 0;
    while(tok != NULL || i > argscount){
        args[i] = malloc(strlen(tok) + 1);
    	args[i] = tok;
		i++;
		if(i == argscount + 1) break;
		tok= strtok(NULL, " ");
    }	
    
    
    char * command = args[0];
	
	if(!strcmp(command,"EOF")) {
	  	status = 0;
 	} else {
 	 	pid_t pid, wpid;
 	 	pid = fork();
		if (pid == 0) {
			executeCommand(args);
		} else {
		// Parent process
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		status = 1;
	  }
  } while (status);
    return 0 ;
}

int executeCommand(char *argv[]){
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	execvp(argv[0],argv);
	return 0;
}
