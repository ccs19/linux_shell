/*
 * =====================================================================================
 *
 *	Authors: Christopher Schneider, Branden Sherrell
 *	File Name: myshell.h
 *	Assignment Number: 1
 *
 *	Description: myshell.c header file
 * =====================================================================================
 */
#ifndef	MYSHELL_H
#define MYSHELL_H


#include "parse.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_INPUT_CHARS		MAXARGS*10	//conservative assumption 10 chars per argument
#define DIR_FILE_LEN		50			//we aren't allocating many Param_t structures, so we can be conservative with the name length
#define EXIT_PARAM			"exit"
#define debug(str){						\
			if(debugMode)				\
				printf("%s", str);		\
		}

/* For boolean clarity */
typedef short bool;
#define true 	1;
#define false 	0;

bool debugMode = false; 


/* Entrance point to the main program */
void shellBegin();

/* Debug function to output parameter information to stdout when in -Debug mode*/
void printParams(Param_t*); 

/* Initializes members of a Param_t structure - no dynamic memory allocation */
void initParam_t(Param_t*);

/*Forks, then executes user input */
void execInput(Param_t*, char*);

/*Attempts to execute child code*/
void execChild(Param_t*, FILE*, FILE*);

/*Returns file stream for redirection*/
FILE *redirectFile(char* , int );

/*Waits for all children to die*/
void waitForChildren();

/*Waits for a specific child to die*/
void parentWait(pid_t, int);

/*Closes open file streams if exec fails*/
void redirectCleanup(FILE*,FILE*);


#endif	//MYSHELL_H
