#ifndef	MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXARGS 		32
#define MAX_INPUT_CHARS	MAXARGS*10	//conservative assumption 10 chars per argument
#define DIR_FILE_LEN	50			//we aren't allocating many Param_t structures, so we can be conservative with the name length
#define EXIT_PARAM		"exit"
#define TOKEN_STR		"<>&-"
#define debug(str){				\
			if(debugMode)				\
				printf("%s", str);	\
		}

/* For boolean clarity */
typedef short bool;
#define true 	1;
#define false 	0;

bool debugMode = false; 

/* Structure to hold input data */
struct PARAM{
	char* inputRedirect; 			//input from file name or NULL
	char* outputRedirect; 			//output to file name or NULL
	int background;					//will work in the background or not
	int argumentCount;				//number of tokens in argumentVector
	char* argumentVector[MAXARGS];	///array of 
};
typedef struct PARAM Param_t; 

/* Entrance point to the main program */
void shellBegin();

/* Debug function to output parameter informtion to stdout when in -Debug mode*/
void printParams(Param_t*); 

/* Takes the input string, tokenizes it, and populates a Param_t structure accordingly */
void tokenizeInput(char*, Param_t*);

/* Allocates member memory for a Param_t (automatic memory) structure */
int newParam_t(Param_t*);

/* Frees member memory for a Param_t (automatic memory) structure */
void freeParam_t(Param_t*);

int execInput(Param_t*, char*);

#endif	//MYSHELL_H














/*
	char* token; 
	char str[] = "myshell -abc -dcf >file.txt &";

	for(int i = 0; i < argc; i++)
		printf("%s\n", argv[i]);

	token = strtok(str, "-&<>");
	while(token != NULL){
		printf("%s\n", token);
		token = strtok(NULL, "-&<>");
	}
*/