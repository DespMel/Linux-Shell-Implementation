#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "p3140116-p3140197-p3140205-mysh.h"

char *read_line(void);
char *concat(char *s1,char *s2);
int main(int argc, char **argv)
{
  char *line;
  char **args;
  int status = 1;

  do {
    printf("mysh1> ");
    line = read_line();
    args = &line;	
  if(!strcmp(*args,"EOF")) {
  	status = 0;
  } else {
  	pid_t pid, wpid;
  	int status;
  	pid = fork();
	if (pid == 0) {
		//Child process
		char cwd[1024];
		getcwd(cwd, sizeof(cwd));
		char *argv[] = {*args,NULL};
		char *env[] = {cwd, NULL};
		char *source = concat("/bin/",*args);
		execve(source,argv,env);
		free(source);
		return 0;
	} else {
		// Parent process
		do {
		  wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
  }

  } while (status);
	return 0 ;
}





