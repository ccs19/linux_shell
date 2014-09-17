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

	//Shell loop
	while(1){
		printf("[myshell]$:  ");
		fgets(inputString, MAX_INPUT_CHARS, stdin);							//this can't fail unless the user never passes parameters 

		if( strncmp(inputString, EXIT_PARAM, sizeof(EXIT_PARAM)-1) == 0 )	//sizeof-1 to neglect \n from stdin, sizeof() is computed a compile-time
			break; 

		initParam_t(&inputInfo);						//init the param_t structure for the next issued system command
		if( tokenizeInput(inputString, &inputInfo) )  	//tokenize the string just read in
			execInput(&inputInfo, inputString);		  	//if ^ returns success, execute the system command

		if(debugMode)
			printParams(&inputInfo);
	}
}/* -----  end of function shellBegin  ----- */


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
		perror("Process fork error");
		exit(EXIT_FAILURE);
	}

	if(child_pid == 0) execChild(param, inFile, outFile);
	else parentWait(child_pid, param->background);
	
}/* -----  end of function execInput  ----- */


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
	
	else if (fileName != NULL)
			f = freopen(fileName, "w", stdout);

	if(f == NULL){			//if(f == NULL && fileName != NULL) -- fileName == NULL will never happen, checkValidRedirect checks this already 
		if(option == INPUT_REDIRECT) perror("File input redirect failed");
		else perror("File output redirect failed");
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
 *         Name: execChild
 *  Description: Attempts to execute child code, calling execvp. If fails, exit child.
 * =====================================================================================
 */
void execChild(Param_t* param, FILE* inFile, FILE* outFile){
	if(param->outputRedirect != NULL)
			outFile = redirectFile(param->outputRedirect, OUTPUT_REDIRECT);

	if(param->inputRedirect != NULL)
		inFile = redirectFile(param->inputRedirect, INPUT_REDIRECT);

	execvp(param->argumentVector[0], param->argumentVector);	//allocates new address space for the child process
	redirectCleanup(inFile, outFile); 							//commented back in to remove warnings
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
	while(wait(NULL) != -1)		//-1 return indicates no "unwaited" children remain
		;
}/* -----  end of function waitForChildren  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name: parentWait
 *  Description: Waits for specific child to terminate based on child_pid. 
 *					If background requested, skips wait.
 * =====================================================================================
 */
void parentWait(pid_t child_pid, int background){
	static int childCount = 0; 
	if(background == 0)
		waitpid( child_pid, NULL, 0 );
	else
		printf("[%d]: %d\n", ++childCount, child_pid);
}/* -----  end of function parentWait  ----- */



