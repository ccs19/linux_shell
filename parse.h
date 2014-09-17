/*
 * =====================================================================================
 *
 *	Authors: Christopher Schneider, Branden Sherrell
 *	File Name: parse.h
 *	Assignment Number: 1
 *
 *	Description: parse.c header file
 * =====================================================================================
 */

#ifndef	PARSE_H
#define PARSE_H

#include <string.h>
#include <stdio.h>

#define INPUT_REDIRECT		0
#define OUTPUT_REDIRECT		1
#define MAXARGS 			32

 /* Structure to hold input data */
struct PARAM{
	char* inputRedirect; 			//input from file name or NULL
	char* outputRedirect; 			//output to file name or NULL
	int background;					//will work in the background or not
	int argumentCount;				//number of tokens in argumentVector
	char* argumentVector[MAXARGS];	///array of 
};
typedef struct PARAM Param_t; 

/* Takes the input string, tokenizes it, and populates a Param_t structure accordingly */
int tokenizeInput(char*, Param_t*);

/* Checks for valid input of user re-directs */
int checkValidRedirect(Param_t* , char*, int);


#endif	//PARSE_H