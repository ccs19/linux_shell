/*
 * =====================================================================================
 *
 *	Authors: Christopher Schneider, Branden Sherrell
 *	File Name: myshell.h
 *	Assignment Number: 1
 *
 *	Description: parse.c implementation file
 * =====================================================================================
 */
//#ifndef	PARSE_H
	#include "parse.h"
//#endif

 /*
 * ===  FUNCTION  ======================================================================
 *         Name: tokenizeInput
 *  Description: Parses user input into strings, recognizing > and < for file 
 *					re-direction, & for backgrounding flag, and  
*					program launch parameters.
 * =====================================================================================
 */
int tokenizeInput(char* str, Param_t* inputInfo){	
	char* token; 

	token = strtok(str, "\n\t\r  ");
	if(token == NULL)	//string was empty
		return 0;
	do{
		switch(*token){
			case '>':
				if(checkValidRedirect(inputInfo, token, OUTPUT_REDIRECT) == 0){
					printf("Invalid output redirect\nSyntax \" > filename \"\n");
					return 0; 
				}
				break;

			case '<':
				if(checkValidRedirect(inputInfo, token, INPUT_REDIRECT) == 0){		
					printf("Invalid input redirect\n");
					return 0;
				}
				break;

			case '&':
				inputInfo->background = 1;
				break;

			default:
				inputInfo->argumentVector[inputInfo->argumentCount] = token;
				inputInfo->argumentCount++;
				break;
		}
		
		token = strtok(NULL, "\n\t\r  ");
	}while(token != NULL); 
	return 1; 
}/* -----  end of function tokenizeInput  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name: checkValidRedirect
 *  Description: Checks for valid re-direction. If failed, returns 0, else 1
 *  		int option:
 *  		option = 0 for inputRedirect
 *  		option = 1 for outputRedirect
 * =====================================================================================
 */
int checkValidRedirect(Param_t* param, char* token, int option){	//need to change the name of this function - it does more than just "check"
	if(option == INPUT_REDIRECT) 
		if(param->inputRedirect == NULL)
			param->inputRedirect = ++token;		//skip to next valid char after symbol
		else{
			printf("Invalid Input: Input Redirection may only be specified once.\n");
			return 0; 
		}

	else if(option == OUTPUT_REDIRECT && param->outputRedirect == NULL) 
			param->outputRedirect = ++token;	//skip to next valid char after symbol
		else{	
			printf("Invalid Input: Output Redirection may only be specified once.\n");
			return 0; 
		}
	return 1;
}/* -----  end of function checkValidRedirect  ----- */
