#include "myshell.h"

int main(int argc, char** argv){
	if(argc > 1){	
		if(strncmp(argv[1], "-Debug", sizeof("-Debug")-1) == 0){	//sizeof-1 to neglect null-char from const string and /n from stdin
			printf("Entered debug mode.\n\n");
			debugMode = true;
		}
	}

	shellBegin();
	//ensure child processes are closed here??
}

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
		newParam_t(&inputInfo);
		tokenizeInput(inputString, &inputInfo);  

		//Testing execution of user input.
		execInput(&inputInfo, inputString);

		if(debugMode)
			printParams(&inputInfo);

		//maybe call function here? - Function to call will be at argumentVector[0]
		freeParam_t(&inputInfo); 
	}
}

void tokenizeInput(char* str, Param_t* inputInfo){	//TODO: if IR, OR, or AV are last in command line then they have an inherent newline character!
	char* token; 	//Could include a function that takes the string to check for newline. Rather ugly solution.
	int file_found; //Check for valid input/output redirect

	token = strtok(str, "\n\t\r  ");
	do{
		file_found = 0;
		switch(*token){


			case '>':
				//Need to search for next characters
				while(file_found == 0){
					token = strtok(NULL, " "); //Check for space characters. Need to protect against newlines and other types of special characters
												//This needs to go into its own function.
					if( token[0] != ' ') file_found = 1;
					printf("%c \n", token[0]);
				}
				inputInfo->outputRedirect = token;

				//if(debugMode)
					//printf("Saved: %s\n", token+sizeof(char));
				break;

			case '<':

				while(file_found == 0){
						token = strtok(NULL, " "); //Check for space characters. Need to protect against newlines and other types of special characters
													//This needs to go into its own function.
						if( token[0] != ' ') file_found = 1;
				}

				inputInfo->inputRedirect = token;

				//if(debugMode)
					//printf("Saved: %s\n", token+sizeof(char));
				break;

			case '&':
				inputInfo->background = 1; 				//true
				//if(debugMode)
					//printf("Background: true\n");
				break;

			default:
				//if(inputInfo->argumentCount == 0){		//if we reach the default && it's the first argument, then it must be the system command or empty string
					inputInfo->argumentVector[inputInfo->argumentCount] = token;
					//strcpy(inputInfo->argumentVector[inputInfo->argumentCount], token);
					inputInfo->argumentCount++;

					//if(debugMode)
						//printf("Saved: %s\n", token);
				//}
				break;
		}
		token  = strtok(NULL, "\n\t\r ");
	}while(token != NULL); 
}

int newParam_t(Param_t* newStruct){
	newStruct->inputRedirect = NULL;			//make sure success or return 0
	newStruct->outputRedirect = NULL; 
	newStruct->argumentCount = 0;
	newStruct->background = 0;
	for(int i = 0; i < MAXARGS; i++)			//optimization potential, set only argumentCount char*'s to NULL - but only MAXARGS exist to begin with
		newStruct->argumentVector[i] = NULL; 	//argument vector char* content allocating is left for when we populate it

	return 1; //allocation success
}

void freeParam_t(Param_t* toFree){
	//for(int i = 0; i < toFree->argumentCount; i++)
		//free(toFree->argumentVector[i]);
	//free(toFree->inputRedirect);
	//free(toFree->outputRedirect);
}

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
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  execInput
 *  Description: Executes user input
 * =====================================================================================
 */
int execInput(Param_t* param, char *str){
	pid_t child_pid, monitor;
	int child_stat;
	child_pid = fork();

	if(child_pid == 0){ //If child is created successfully, attempt to execute

		//debugging line
		printf("arg0: %s\n arg1: %s\n", param->argumentVector[0], param->argumentVector[1]);

		execvp(param->argumentVector[0], param->argumentVector);

		//Error thrown if invalid command.  Perror allows program executed to throw its own error, if applicable.
		perror("Invalid input: ");
		exit(0);
	}
	else{

		do{
			monitor = wait(&child_stat);
		}while(monitor != child_pid);
	}

	return child_stat;

}/* -----  end of function openFile  ----- */