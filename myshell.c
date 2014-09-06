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
 *  Description:
 * =====================================================================================
 */
int main(int argc, char** argv){
	if(argc > 1)
		if(strncmp(argv[1], "-Debug", sizeof("-Debug")-1) == 0){	//sizeof-1 to neglect null-char from const string and /n from stdin
			printf("Entered debug mode.\n\n");
			debugMode = true;
		}

	shellBegin();
	//ensure child processes are closed here??
}/* -----  end of function main  ----- */



/*
 * ===  FUNCTION  ======================================================================
 *         Name: shellBegin
 *  Description:
 * =====================================================================================
 */
void shellBegin(){
	char inputString[MAX_INPUT_CHARS];	//temp location to hold input string 
	Param_t inputInfo; 
	
	while(1){
		printf("[myshell]$:  ");
		fgets(inputString, MAX_INPUT_CHARS, stdin);							//this can't fail unless the user never passes paramters 
		if( strncmp(inputString, EXIT_PARAM, sizeof(EXIT_PARAM)-1) == 0 ){	//sizeof-1 to neglect null-char from const string and /n from stdin
			//ensure child processes are stopped before breaking
			break; 
		}
		initParam_t(&inputInfo);
		if( tokenizeInput(inputString, &inputInfo) )  
			execInput(&inputInfo, inputString);	//Testing execution of user input.

		if(debugMode)
			printParams(&inputInfo);

	}
}/* -----  end of function shellBegin  ----- */



/*
 * ===  FUNCTION  ======================================================================
 *         Name: tokenizeInput
 *  Description:
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
 *         Name: newParam_t
 *  Description:
 * =====================================================================================
 */
void initParam_t(Param_t* newStruct){
	newStruct->inputRedirect = NULL;			
	newStruct->outputRedirect = NULL; 
	newStruct->argumentCount = 0;
	newStruct->background = 0;
	for(int i = 0; i < MAXARGS; i++)			//optimization potential, set only argumentCount char*'s to NULL before setting to 0
		newStruct->argumentVector[i] = NULL; 	//argument vector char* memory allocating is left for when we populate it this field
}/* -----  end of function initParam_t  ----- */




/*
 * ===  FUNCTION  ======================================================================
 *         Name: printParams
 *  Description:
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
 *  Description: Executes user input
 * =====================================================================================
 */
int execInput(Param_t* param, char *str){
	pid_t child_pid, monitor;
	int child_stat = 0;
	FILE* outFile = NULL; 
	FILE* inFile = NULL; 

	child_pid = fork();		//returns new child PID in parent process and 0 for child process
	if(child_pid < 0){		//fork failure
		perror("Error");
		exit(EXIT_FAILURE);
	}

	if(child_pid == 0){ //If child is created successfully, attempt to execute
		if(param->outputRedirect != NULL)outFile = redirectFile(param->outputRedirect, OUTPUT_REDIRECT);
		if(param->inputRedirect != NULL) inFile = redirectFile(param->inputRedirect, INPUT_REDIRECT);

		execvp(param->argumentVector[0], param->argumentVector);	//replaces memory space with a new program (destroying duplicate created from its parent)
		redirectCleanup(inFile, outFile);
		perror("Invalid input");
		exit(EXIT_FAILURE);
	}

	else{
		if(param->background == 0){
			do{								//We need to conitnue to call wait() in order to free child pids and retrieve exit-status info to prevent "zombie processes"
				monitor = wait(NULL);		//if not null, status information is assigned to the int passed in (wait removes calling process from ReadyQueue)
			}while(monitor != child_pid);	//wait() returns the pid of terminated child process
		}

	}

	return child_stat; // Return not needed

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
int checkValidRedirect(Param_t* param, char* token, int option){
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

	if(option == INPUT_REDIRECT)
		f = freopen(fileName, "r", stdin);
	else f = freopen(fileName, "w", stdout);

	if(f == NULL){
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









