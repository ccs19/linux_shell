/*
 * =====================================================================================
 *
 *	Authors: Christopher Schneider, Branden Sherrell
 *	File Name: myshell.c
 *	Assignment Number: 1
 *
 *	Description: Main code for a linux shell.
 * =====================================================================================
 */



#include "myshell.h"



/*
 * ===  FUNCTION  ======================================================================
 *         Name: main
 *  Description: Checks for debug mode, then launches shell
 * =====================================================================================
 */
int main(int argc, char** argv){
	if(argc > 1)
		if(strncmp(argv[1], "-Debug", sizeof("-Debug")-1) == 0){	//sizeof-1 to neglect null-char from const string and /n from stdin
			printf("Entered debug mode.\n\n");
			debugMode = true;
		}

	shellBegin();
	waitForChildren();
	exit(EXIT_SUCCESS);

}/* -----  end of function main  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name: shellBegin
 *  Description: Starts the shell and continues in a loop until EXIT_PARAM is 
 *					entered.
 * =====================================================================================
 */
void shellBegin(){
	char inputString[MAX_INPUT_CHARS];	//temp location to hold input string 
	Param_t inputInfo; 
	commHistory.toExecute = NULL;		//init global command history structure 
	commHistory.comm_index = 0; 
	//Shell loop
	while(1){
		if(commHistory.toExecute != NULL){	//if we have specified previously we want to execute a previous command
			strncpy(inputString, commHistory.toExecute, strlen(commHistory.toExecute)+1);	//+1 for null-char 
			commHistory.toExecute = NULL;
		}else{
			printf("[myshell]$:  ");
			fgets(inputString, MAX_INPUT_CHARS, stdin);							//this can't fail unless the user never passes parameters 
			updateCommandHistory(inputString); 

			if( strncmp(inputString, EXIT_PARAM, sizeof(EXIT_PARAM)-1) == 0 ){	//sizeof-1 to neglect \n from stdin
				break; 
			}
		}
		initParam_t(&inputInfo);
		if( tokenizeInput(inputString, &inputInfo) )  
			execInput(&inputInfo, inputString);	

		if(debugMode)
			printParams(&inputInfo);
	}
}/* -----  end of function shellBegin  ----- */



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

			case '!':
				token++;
				if(*token == '!')					//means string was "!!"
					commandHistory(LIST_COMMANDS);
				else if(*token != '\0')				//not end of string, at least one more character
					commandHistory(atoi(token));	//assume integer - error checking in function
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
 *         Name: initParam_t
 *  Description: Initializes data structure for user input
 * =====================================================================================
 */
void initParam_t(Param_t* newStruct){
	newStruct->inputRedirect = NULL;			
	newStruct->outputRedirect = NULL; 
	newStruct->argumentCount = 0;
	newStruct->background = 0;
	for(int i = 0; i < MAXARGS; i++)			
		newStruct->argumentVector[i] = NULL; 	
}/* -----  end of function initParam_t  ----- */




/*
 * ===  FUNCTION  ======================================================================
 *         Name: printParams
 *  Description: Prints data stored in a Param_t structure.
 * =====================================================================================
 */
void printParams(Param_t* param){
	int i; 
	printf("\n\nInputRedirect: [%s]\n", 
		(param->inputRedirect != NULL) ? param->inputRedirect:"NULL");
	printf("OutputRedirect: [%s]\n", 
		(param->outputRedirect != NULL) ? param->outputRedirect:"NULL");
	printf("Background: [%d]\n", param->background);
	printf("ArgumentCount: [%d]\n", param->argumentCount); 
	for(i = 0; i < param->argumentCount; i++)
		printf("ArgumentVector[%2d]: [%s]\n", i, param->argumentVector[i]);
	puts("");
}/* -----  end of function printParams ----- */




/*
 * ===  FUNCTION  ======================================================================
 *         Name:  execInput
 *  Description: Forks, then executes input based on child or parent.
 * =====================================================================================
 */
void execInput(Param_t* param, char *str){
	pid_t child_pid;
	FILE* outFile = NULL; 
	FILE* inFile = NULL; 
	child_pid = fork();		//returns new child PID in parent process and 0 for child process
	
	if(child_pid < 0){		//fork failure
		perror("Error");
		exit(EXIT_FAILURE);
	}

	if(child_pid == 0) execChild(param, inFile, outFile);
	else parentWait(child_pid, param->background);
	
}/* -----  end of function execInput  ----- */




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







/*
 * ===  FUNCTION  ======================================================================
 *         Name: redirectFile
 *  Description: Returns file pointer for re-direction
 * =====================================================================================
 */

FILE *redirectFile(char* fileName, int option){
	FILE *f = NULL;

	if(option == INPUT_REDIRECT && fileName != NULL)
		f = freopen(fileName, "r", stdin);
	else if (fileName != NULL) f = freopen(fileName, "w", stdout);

	if(f == NULL && fileName != NULL){
		if(option == INPUT_REDIRECT) perror("File input redirect failed.");
		else perror("File output redirect failed.");
	}
	return f;
}/* -----  end of function redirectFile  ----- */







/*
 * ===  FUNCTION  ======================================================================
 *         Name: redirectCleanup
 *  Description: closes file redirection streams if they exist.
 * =====================================================================================
 */
void redirectCleanup(FILE *in, FILE *out){

	//If either files exist, close them.
	if (out != NULL)
		fclose(out);

	if(in != NULL)
		fclose(in);

}/* -----  end of function redirectCleanup  ----- */






/*
 * ===  FUNCTION  ======================================================================
 *         Name: commandHistory
 *  Description: Lists BUFF_SIZE number of previous commands
 * =====================================================================================
 */
void commandHistory(int commandNum){
	if(commandNum < 0)
		for(int i = commHistory.comm_index; i > 0; i--)
			printf("!%d: %s", i, commHistory.command[i]); 	//newline present from stdin 
	else{
		if( (commandNum == 0) || (commandNum > BUFF_SIZE) ){
			printf("Error: Please specify command index for [1,%d]\n", BUFF_SIZE);
			return;
		}
		commHistory.toExecute = commHistory.command[commandNum];
	}
}/* -----  end of function commandHistory  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name: execChild
 *  Description: Attempts to execute child code, calling execvp. If fails, exit child.
 * =====================================================================================
 */
void execChild(Param_t* param, FILE* inFile, FILE* outFile){
			if(param->outputRedirect != NULL)
				outFile = redirectFile(param->outputRedirect, OUTPUT_REDIRECT);

			if(param->inputRedirect != NULL)
				inFile = redirectFile(param->inputRedirect, INPUT_REDIRECT);

			execvp(param->argumentVector[0], param->argumentVector);	//replaces memory space with a new program (destroying duplicate created from its parent)
			redirectCleanup(inFile, outFile); //commented back in to remove warnings
			perror("Invalid input");
			exit(EXIT_FAILURE);

}/* -----  end of function execChild  ----- */

/*
 * ===  FUNCTION  ======================================================================
 *         Name: waitForChildren
 *  Description: Attempts to execute child code, calling execvp. If fails, exit child.
 * =====================================================================================
 */
void waitForChildren(){
	pid_t no_zombie;
	while(no_zombie != -1){
		no_zombie = wait(NULL); //Wait returns -1 if no child process exists
	}
}/* -----  end of function waitForChildren  ----- */



/*
 * ===  FUNCTION  ======================================================================
 *         Name: parentWait
 *  Description: Waits for specific child to terminate based on child_pid. 
 *					If background requested, skips wait.
 * =====================================================================================
 */
void parentWait(pid_t child_pid, int background){
	if(background == 0) waitpid( child_pid, NULL, 0 );
}/* -----  end of function parentWait  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name: updateCommandHistory
 *  Description: 
 * =====================================================================================
 */
void updateCommandHistory(const char* str){
	commHistory.comm_index = (commHistory.comm_index+1)%BUFF_SIZE; 
	strncpy(commHistory.command[commHistory.comm_index], str, strlen(str)+1);	//copy command into history buffer, +1 to copy null-char! 
}/* -----  end of function updateCommandHistory  ----- */










