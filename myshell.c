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
	if(argc > 1){	
		if(strncmp(argv[1], "-Debug", sizeof("-Debug")-1) == 0){	//sizeof-1 to neglect null-char from const string and /n from stdin
			printf("Entered debug mode.\n\n");
			debugMode = true;
		}
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
		tokenizeInput(inputString, &inputInfo);  

		//Testing execution of user input.
		execInput(&inputInfo, inputString);

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
void tokenizeInput(char* str, Param_t* inputInfo){	
	char* token; 

	token = strtok(str, "\n\t\r  ");
	if(token == NULL)	//string was empty
		return;

	do{
		switch(*token){
			case '>':
				if(checkValidRedirect(inputInfo, token, OUTPUT_REDIRECT) == 0)		
					printf("Invalid output redirect\nSyntax \" > filename \"\n");
				break;

			case '<':
				if(checkValidRedirect(inputInfo, token, INPUT_REDIRECT) == 0)		
				printf("Invalid input redirect\n");
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

	child_pid = fork();		//returns new child PID in parent process and 0 for child process
	if(child_pid < 0){		//fork failure
		perror("Error");
		exit(EXIT_FAILURE);
	}

	if(child_pid == 0){ //If child is created successfully, attempt to execute
		if(param->outputRedirect != NULL){
			outFile = freopen(param->outputRedirect, "w", stdout);

			if(outFile == NULL)	//stream association failed
				perror("Error");
		}

		execvp(param->argumentVector[0], param->argumentVector);
		if(outFile != NULL)	//close only if we associated it to begin with, otherwise this is undefined behavior so the check is required
			fclose(outFile);

		//Error thrown if invalid command.  Perror allows program executed to throw its own error, if applicable.
		perror("Invalid input");
		exit(EXIT_FAILURE);
	}
	else{
		do{
			monitor = wait(NULL);		//if not null, status information is assigned to the int passed in
		}while(monitor != child_pid);	//wait() returns the pid of terminated child process
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
	while(1){
		token++;
		if(token == NULL) return 0;
		if(option == INPUT_REDIRECT) param->inputRedirect = token;
		else if(option == OUTPUT_REDIRECT) param->outputRedirect = token;
		return 1;
	}
}/* -----  end of function checkValidRedirect  ----- */



/*Adds user command to buffer*/
//void addToBuffer(char* c){
//
//	command_buffer[numCommands] = malloc(sizeof(char)*(strlen(c)));
//	if(numCommands < BUFF_SIZE ) numCommands++;
//
//}

void initBuffer(){

}
