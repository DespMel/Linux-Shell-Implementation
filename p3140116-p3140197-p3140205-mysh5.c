#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "p3140116-p3140197-p3140205-mysh.h"
#define _GNU_SOURCE



int executeCommand(char *args[], int length);
int countCharsInLine(char * line, char c);
void trim(char * s); 
void executeUserCommand(int in, int out, char * line);
char *read_line(void);
char *concat(char *s1,char *s2);


int main(int argc, char **argv)
{
  char *line;
  int status = 1;
  int screen_in = dup(0);
  int screen_out = dup(1);
  int screen_err = dup(2);
  do {
        printf("mysh5> ");
        line = read_line();
     	
	int pipeCommands = countCharsInLine(line, '|');
        
	if(pipeCommands>0) {
	  
		int in = screen_in;
		int out = screen_out;
		char * pc[pipeCommands+1];
		pc[0] = strtok(line, "|");
	    for(int i = 1; i<=pipeCommands; i++){
	        pc[i] = strtok(NULL, "|");
	    }
		
		int prev_in = screen_in;
		for(int i = 0; i<=pipeCommands; i++){
		    char* pipeCommand = pc[i];
		    trim(pipeCommand);
		
			
			int pipefd[2];
			if(i<pipeCommands){
				pipe(pipefd);
				out = pipefd[1];
				
			} else {
				out = screen_out;
			}
			executeUserCommand(in, out, pipeCommand);
			
			if(i<pipeCommands){
				if(prev_in!=screen_in){
					close(prev_in);
				}
				prev_in = in;
				in = pipefd[0];
				close(pipefd[1]);
			}
		}
	}
	else {
		executeUserCommand(screen_in, screen_out,line);
	}
	dup2(screen_in, 0);
	dup2(screen_out,1);
	dup2(screen_err,2);
	status = 1;
	
    
  } while (1);
    return 0 ;
}

void executeUserCommand(int in, int out, char * line) {

	int status = 1;
	int i, argscount;
        int inputfile = countCharsInLine(line, '<');
	int outputfile = countCharsInLine(line, '>');
	if(inputfile){
		char * tok;
		tok = strtok(line, "<");
		if(outputfile){
			char * command[3];
			command[0] = tok;
			tok = strtok(NULL, "<");
			command[1] = strtok(tok, ">");
			command[2] = strtok(NULL, ">");
			trim(command[0]);
			trim(command[1]);
			trim(command[2]);
			pid_t pid, wpid;
	 	 	pid = fork();
			if (pid == 0) {
				int fd = open(command[1], O_RDONLY);
				int fd_out = open(command[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				dup2(fd, 0);
				dup2(fd_out, 1);
				dup2(fd_out, 2);
				close(fd);
				close(fd_out);
				argscount = countCharsInLine(command[0], ' ');
				char *args[argscount +2];
				args[argscount + 1] = NULL;
				tok = strtok(command[0], " ");
				i = 0;
				while(tok != NULL || i > argscount){
					args[i] = malloc(strlen(tok) + 1);
					args[i] = tok;
					i++;
					if(i == argscount + 1) break;
					tok= strtok(NULL, " ");
				}	
				executeCommand(args, argscount + 2);
				
				
			} else {
			// Parent process
				do {
					wpid = waitpid(pid, &status, WUNTRACED);
				} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			}
		}	
		else{
			char * command[2];
			command[0] = tok;
			command[1] = strtok(NULL, "<");
			trim(command[0]);
			trim(command[1]);
			pid_t pid, wpid;
	 	 	pid = fork();
			if (pid == 0) {
				int fd = open(command[1], O_RDONLY);
				dup2(fd, 0);
				dup2(out,1);
				close(fd);
				argscount = countCharsInLine(command[0], ' ');
				char *args[argscount +2];
				args[argscount + 1] = NULL;
				tok = strtok(command[0], " ");
				i = 0;
				while(tok != NULL || i > argscount){
					args[i] = malloc(strlen(tok) + 1);
					args[i] = tok;
					i++;
					if(i == argscount + 1) break;
					tok= strtok(NULL, " ");
				}	
				executeCommand(args, argscount + 2);
				
			} else {
			// Parent process
				do {
					wpid = waitpid(pid, &status, WUNTRACED);
				} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			}
		}
	}
	else if(outputfile){
		char * tok;
		tok = strtok(line, ">");
		char * command[2];
		command[0] = tok;
		command[1] = strtok(NULL, ">");
		trim(command[0]);
		trim(command[1]);
		pid_t pid, wpid;
 	 	pid = fork();
		if (pid == 0) {
			int fd = open(command[1],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(in, 0);
			dup2(fd, 1);
			dup2(fd, 2);
			argscount = countCharsInLine(command[0], ' ');
			trim(command[0]);
			close(fd);
			char *args[argscount +2];
			args[argscount + 1] = NULL;
			tok = strtok(command[0], " ");
			i = 0;
			while(tok != NULL || i > argscount){
				args[i] = malloc(strlen(tok) + 1);
				args[i] = tok;
				i++;
				if(i == argscount + 1) break;
				tok= strtok(NULL, " ");
			}	
			executeCommand(args, argscount + 2);
			
		} else {
		// Parent process
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
	else {
		trim(line);
		argscount = countCharsInLine(line, ' ');
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
				dup2(in, 0);
				dup2(out, 1);
				execvp(args[0], args);
			} else {
			// Parent process
				do {
					wpid = waitpid(pid, &status, WUNTRACED);
				} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			}
		  }
	}
}

int countCharsInLine(char * line, char c){
	int argscount = 0;
	char *pch = strchr(line, c);
	while(pch != NULL){
		argscount++;
		pch = strchr(pch + 1, c);	
	}
	return argscount;
}

int executeCommand(char *argv[], int length){
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	char *source = concat("/bin/", argv[0]);
	execvp(argv[0], argv);
	return 0;
}

void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;
    memmove(s, p, l + 1);
}   






