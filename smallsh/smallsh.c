// Author: Ryan Shin
// 8/04/17
// Desc: Program 3 smallsh.c.
// This program is a basic shell.
//
// It can run 3 native commands: "status" for the status code of 
// previously terminated process. "cd" to change directory and 
// "exit" to exit. It can also run other UNIX commands through
// exec calls. 


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define false 0
#define true 1
#define COMMAND_SIZE 2048
#define ARG_SIZE 512
#define PROCESS_MAX 256

//set if you are me and want trace statements.
#define DEBUG 0

//function prototypes
char *getUserInput();
void catchSIGTSTP(int signo);
void catchSIGINT(int signo);
void removeArray(pid_t process[], int idx, int *psize);
int replaceFirst$$(char** bufferA);

//global variables
int foregroundOnly;
int SIGTSTP_Handled;
pid_t process[PROCESS_MAX];

void main(){
    //flag to control main loop
    int exitFlag = false;
    //holds status code or exit code
    int statusCode = -5;
    //boolean to detrermine if statusCode is a signal code or an exit code
    int statusIsSigno = false;
    //process array size
    int psize = 0;

    //signal handlers
    struct sigaction default_action = {0}, SIGTSTP_action = {0}, ignore_action = {0};

    default_action.sa_handler = SIG_DFL;
    sigfillset(&default_action.sa_mask);
    default_action.sa_flags = 0;

    SIGTSTP_action.sa_handler = catchSIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = 0;

    ignore_action.sa_handler = SIG_IGN;

    //Ignore SIGINT and catch and use unique handler for SIGTSTP
    sigaction(SIGINT, &ignore_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    //boolean to control foregroundOnly mode.
    foregroundOnly = false;
    //input buffer
    char *buffer;

    //main shell loop
    while(!exitFlag){
        //This array will hold tokens of a comand.
        char commands[ARG_SIZE][COMMAND_SIZE];
        //This array will hold arguments of a single command
        char* args[ARG_SIZE];
        char inputpath[512];
        char outputpath[512];

        int i;
        //memset all strings
        memset(inputpath, '\0', 512);
        memset(outputpath, '\0', 512);
        for (i=0; i<ARG_SIZE; ++i){
            memset(commands[i], '\0', COMMAND_SIZE);
        }

        //reset flags
        int outputRFlag = false;
        int inputRFlag = false;
        int backgroundP = false;
        SIGTSTP_Handled = false;

        //check for completed processes
        int backgroundExitStatus;

        //If any have been completed, print pid and either exit value or signal number. Remove from array of processes.
        for (i=0; i<psize; ++i){
            if (waitpid(process[i], &backgroundExitStatus, WNOHANG) > 0){
                if (WIFEXITED(backgroundExitStatus)){
                    printf("background pid %d is done: exit value %d\n", process[i], WEXITSTATUS(backgroundExitStatus));
                }
                if (WIFSIGNALED(backgroundExitStatus)){
                    printf("background pid %d is done: terminated by signal %d\n", process[i] , WTERMSIG(backgroundExitStatus));
                }
                removeArray(process, i, &psize);
                if (DEBUG) {
                        printf("psize: %d\n", psize);
                        for (i=0; i<psize; ++i) printf ("%d\t", process[i]);
                }
            }
        }

        //get user input and show shell prompt symbol ": "
        buffer = getUserInput();

        //if signalstop was handled skip to end of loop.
        if(SIGTSTP_Handled){
        //if input is empty, skip to the end of the loop.
        } else if(strcmp(buffer, "") == 0){
        } else {
            int csize = 0;
            char *token;

            // replace all instances of $$ with pid.
            while(replaceFirst$$(&buffer)){ }

            //parse input into tokens
            token = strtok(buffer, " ");
            strcpy(commands[csize++], token);

            token = strtok(NULL, " ");

            while(token != NULL) {
                strcpy(commands[csize++], token);
                token = strtok(NULL, " ") ;
            }

            //check if command is comment
            if (commands[0][0] == '#');
            //check if command is 'exit'
            else if(strcmp(commands[0], "exit") == 0){
                //if so, exit loop
                exitFlag = true;
            } 
            //if command is status, show last status
            else if(strcmp(commands[0], "status") == 0){
                if(statusIsSigno){
                    printf("terminated by signal %d\n", statusCode);
                    fflush(stdout);
                } else {
                    printf("exit value %d\n", statusCode);
                    fflush(stdout);
                }
                // if command is cd
            } else if(strcmp(commands[0], "cd") == 0){
                //check next token for path
                if (strcmp(commands[1], "") == 0){
                    strcpy(commands[1], getenv("HOME"));
                    if(DEBUG) printf("%s\n", getenv("HOME"));
                }
                //change directory and handle errors
                if(chdir(commands[1]) == -1) {
                    fprintf(stderr, "cd: %s: no such file or directory\n", commands[1]);
                    fflush(stderr);
                }
            //otherwise create child and have child exec command.
            } else {
                int i;
                //check for & at the end of command.
                if (strcmp(commands[csize-1], "&") == 0){
                    //if not in foreground-only mode, set background process flag.
                    if(!foregroundOnly){
                        backgroundP = true;
                        if(DEBUG) printf("Found Background Token.\n");
                    }   
                    //remove & token from array.
                    csize--;
                }
                // only collect argument tokens until > or < is encountered.
                for(i=0; i<csize; ++i) {
                    if (strcmp(commands[i], ">") == 0 || strcmp(commands[i], "<") == 0 ) break;
                    args[i] = commands[i];
                }
                //null terminate the list of arguments to prepare for execvp
                args[i] = NULL;

                //set redirection flags and input and output filepaths
                while ( i < csize && (strcmp(commands[i], ">") == 0 || strcmp(commands[i], "<") == 0))
                {
                    if (strcmp(commands[i], "<") == 0){
                        i++;
                        if (i < csize){
                            strcpy(inputpath, commands[i]);
                            if(DEBUG) printf("Redirecting stdin to %s.\n", inputpath);
                            i++;
                            inputRFlag = true;
                        }
                    } else {
                        i++;
                        if (i < csize){
                            strcpy(outputpath, commands[i]);
                            if(DEBUG) printf("Redirecting stdout to %s.\n", outputpath);
                            i++;
                            outputRFlag = true;
                        }
                    }
                }


                pid_t myPid  = -10;
                int childExitStatus = -10;

                //fork process
                myPid = fork();
                switch (myPid){
                    case -1: { perror("Hull Breach!\n"); exit(1); break; }
                    case 0: {
                                //set default action for SIGINT for foreground child processes
                                if (!backgroundP) sigaction(SIGINT, &default_action, NULL);
                                //ignore SIGTSTP for all child processes.
                                sigaction(SIGTSTP, &ignore_action, NULL);

                                //set redirection
                                if(inputRFlag){
                                    int inFD = open(inputpath, O_RDONLY, 0644);
                                    if (inFD == -1) { printf("cannot open %s for input\n", inputpath); exit(1);}
                                    fflush(stdout);
                                    int result = dup2(inFD, 0);
                                    if (result == -1) { printf("dup2()"); exit(1);}
                                    fflush(stdout);
                                    close(inFD);
                                }
                                if(outputRFlag){
                                    int outFD = open(outputpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                    if (outFD == -1) { printf("cannot open %s for output\n", outputpath); exit(1);}
                                    fflush(stdout);
                                    int result = dup2(outFD, 1);
                                    if (result == -1) { perror("dup2()"); exit(1);}
                                    fflush(stdout);
                                    close(outFD);
                                }

                                //if background process and either redirection flags were not set,
                                //set stdin and/or stdout to dev/null.
                                if(backgroundP){
                                    if(!inputRFlag){
                                        //set stdin to /dev/null
                                        int inFD = open("/dev/null", O_RDONLY, 0644);
                                        int result = dup2(inFD, 0);
                                        if (result == -1) { printf("dup2()"); exit(1);}
                                        fflush(stdout);
                                        close(inFD);
                                    }
                                    if(!outputRFlag){
                                        //set stdout to /dev/null
                                        int outFD = open("/dev/null", O_WRONLY, 0644);
                                        int result = dup2(outFD, 1);
                                        if (result == -1) { printf("dup2()"); exit(1);}
                                        fflush(stdout);
                                        close(outFD);
                                    }
                                }

                                //execute the command
                                execvp(args[0], args);
                                //give error message if command fails
                                fprintf(stderr, "%s: no such file or directory\n", args[0]);
                                fflush(stderr);
                                exit(1); break; 
                            }
                    default: {
                                 if(backgroundP){
                                     // add background process to array
                                     process[psize] = myPid;
                                     psize++;
                                     waitpid(myPid, &childExitStatus, WNOHANG);
                                     //print background process pid
                                     printf("background pid is %d\n", myPid);
                                     fflush(stdout);
                                 } else {
                                     //if it is foreground process, wait on process
                                     pid_t actualPid = waitpid(myPid, &childExitStatus, 0);
                                     // and set exit status
                                     if (WIFEXITED(childExitStatus)){
                                         statusCode = WEXITSTATUS(childExitStatus);
                                         statusIsSigno = false;
                                     }
                                     if (WIFSIGNALED(childExitStatus)){
                                         statusCode = WTERMSIG(childExitStatus);
                                         statusIsSigno = true;
                                         if(statusCode <= 100) printf("terminated by signal %d\n", statusCode);
                                         fflush(stdout);
                                     } else {
                                     }
                                 }
                             }
                }
            }
        }
        //deallocate user input buffer
        if (buffer != NULL) free(buffer);
    }
}

// The following function returns a dynamically allocated
// buffer containing a user inputted line, without the newline
// character.
char *getUserInput(){
    size_t bufferSize = 0;
    char* buffer = NULL;

    printf(": ");
    fflush(stdout);

    //get input and store in dynamically allocated c-string
    getline(&buffer, &bufferSize, stdin);

    //clear error flags incase of signal interrupts
    clearerr(stdin);
    fflush(stdin);

    //if signal was set during input, exit.
    if(SIGTSTP_Handled) return NULL;

    //remove newline char
    buffer[strcspn(buffer, "\n")] = '\0';

    return buffer;
}

// The catchSIGTSP function catches SIGTSTP and
// switches the foregroundOnly mode.
void catchSIGTSTP(int signo){
    //activate foreground only mode.
    SIGTSTP_Handled = true;
    if (foregroundOnly == false){
        puts("\nEntering foreground-only mode (& is now ignored)");
        fflush(stdout);
        foregroundOnly = true;
    } else {
        foregroundOnly = false;
        puts("\nExiting foreground-only mode");
        fflush(stdout);
    }
}

//The removeArray function removes the indexed value from the array
//and shifts the elements so that no gap remains. psize is decremented.
void removeArray(pid_t process[], int idx, int *psize){
    //idx holds index to be removed.
    int j;
    for (j=idx; j<*psize-1; ++j){
        process[j] = process[j+1];
    }
    (*psize)--;
}

//The replaceFirst$$ function replaces the first instance
//of "$$" with the process id of the caller process. It is
//passed a pointer to a buffer, so that the buffer can point
//to the newly created string.
int replaceFirst$$(char** bufferAddress){
    char *buffer = *bufferAddress;
    pid_t pid = getpid();
    char pidString[32];
    //convert integer pid to a string.
    sprintf(pidString, "%d", pid);
    //calculate the size of the new buffer.
    int size = strlen(buffer) + strlen(pidString) - 2;

    //allocate a new buffer
    char* newBuffer = malloc(sizeof(char)*size);

    //find and replace first $$. 
    char* pointer = strstr(buffer, "$$");
    if (pointer == NULL) {
        return false;
    }
    int offset = pointer - buffer;

    char string1[COMMAND_SIZE];
    char string2[COMMAND_SIZE];

    //copy first splice of the string.
    strncpy(string1, buffer, offset);
    string1[offset] = '\0';
    strcpy(string2, pointer+2);

    //concatenate the process id
    strcat(string1, pidString);
    //concatenate the second splice
    strcat(string1, string2);
    //copy the new string to the new buffer.
    strcpy(newBuffer, string1);
    //free old buffer
    free(*bufferAddress);
    //let the old buffer point to the new buffer
    *bufferAddress = newBuffer;
    return true;
}

