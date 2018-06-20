// Author: Ryan Shin
// 8/18/17
// CS344
// Assignment 4- OTP
// File: otp_dec_d.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//This macro is adapted from www.lemoda.net/c/modulo-operator/
#define MOD(a,b) ((((a)%(b))+(b))%(b))
#define BUFF_SIZE 100000
#define DEBUG 0
#define PROCESS_MAX 5

//Prints out the ASCII characters of a string. Used for debugging.
void printStringContents(char *buffer){
    int x = 0;
    printf("CHAR INT\n");
    for (x = 0; x < strlen(buffer); x++)
        printf (" \'%c\'  %d\n", buffer[x], buffer[x]);
    printf("---------\n");
}
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

//This function removes a process from the array.
void removeArray(pid_t process[], int idx, int *psize){
    //idx holds index to be removed.
    int j;
    for (j=idx; j<*psize-1; ++j){
        process[j] = process[j+1];
    }
    (*psize)--;
}

// This function will decrypt the OTP code.
int decrypt (char *buffer, char *key){
    if (strlen(buffer) > strlen(key)) {
        return -1;
    }
    else {
        int i;
        int val;
        //array to hold alphabet index map.
        char alph[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
        int b;
        int k;
        for (i=0; i<strlen(buffer); i++){
            if (buffer[i] == ' ') b = 26;
            else b = buffer[i] - 'A';
            if (key[i] == ' ') k = 26;
            else k = key[i] - 'A';
            //use special MOD macro since C's modulus doesnt work well with negatives
            val = MOD((b - k), 27);
            buffer[i] = alph[val];
        }
        return 0;
    }
}

// This fucntion sends the string in buffer ending with a terminal character '@@'
// to the socket specified in socketFD. 
void sendMsg(char *buffer, int socketFD){
    int bytesSent = 0;
    int ret;
    //add terminal characters
    strcat(buffer, "@@");

    int fileLength = strlen(buffer);

    //send file contents
    while (bytesSent < fileLength ) {
        ret = send(socketFD, buffer + bytesSent, fileLength - bytesSent, 0);
        if (bytesSent < 0) {error("CLIENT: ERROR writing to socket"); break;}
        else bytesSent += ret;
    }
}

// This fucntion receives a string message through the socketFD,
// and constructs a string completeMessage, stopping when a terminal character '@@'
// is received. 'buffer' will contain a part of that message after the function terminates.
void recMsg(char *completeMessage,char *buffer, int socketFD){
    int charsRead = 0;
    memset(completeMessage, '\0', sizeof(completeMessage)); // Clear out the buffer again for reuse
    while (strstr(completeMessage, "@@") == NULL){
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
        strcat(completeMessage, buffer);
        if (charsRead < 0) error("CLIENT: ERROR reading from socket");
    }
    completeMessage[strcspn(completeMessage, "@")] = '\0';
    if(DEBUG){
        printf("SERVER: I received this from the client: \"%s\"\n", completeMessage);
        printStringContents(completeMessage);
    }
}

int main(int argc, char *argv[])
{

    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    char buffer[BUFF_SIZE];
    char completeMessage[BUFF_SIZE];
    char key[BUFF_SIZE];
    struct sockaddr_in serverAddress, clientAddress;

    if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

    // Set up the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (listenSocketFD < 0) error("ERROR opening socket");

    // Enable the socket to begin listening
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
        error("ERROR on binding");
    listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    //Reap finished processes
    int i; 
    pid_t process[PROCESS_MAX];
    int psize = 0;

    int backgroundExitStatus;


    // Main server loop
    while (1){
        pid_t myPid  = -10;
        int childExitStatus = -10;

        // Check for fininished child processes to reap.  
        for (i=0; i<psize; i++){
            if (waitpid(process[i], &backgroundExitStatus, WNOHANG) > 0){
                removeArray(process, i, &psize);
                if (DEBUG) {
                    printf("psize: %d\n", psize);
                    for (i=0; i<psize; i++) printf ("%d\t", process[i]);
                    if(DEBUG) printf("Child process reaped!\n");
                }
            }
        }

        if (psize >= 5){
            perror("SERVER: Currently handling maximum number of requests. Please wait.");
            fflush(stderr);

        } else {
            // Accept a connection, blocking if one is not available until one connects
            sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
            establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
            if (establishedConnectionFD < 0) error("ERROR on accept");

            myPid = fork();
            switch (myPid){
                case -1: { perror("Hull Breach!\n"); exit(1); break; }
                case 0: {
                            // Child process
                            if(DEBUG) printf("Child process created!\n");

                            //Check authentication
                            memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
                            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
                            if (charsRead < 0) error("CLIENT: ERROR reading authentication from socket");

                            if (strcmp(buffer, "dec") != 0) {
                                if(DEBUG) printf("Authentication failed! Client sent '%s'\n", buffer);
                                char authMsg[] = "no";
                                sendMsg(authMsg, establishedConnectionFD);
                            }
                            else {
                                char authMsg[] = "yes";
                                sendMsg(authMsg, establishedConnectionFD);

                                // Get the message from the client and display it
                                recMsg(completeMessage, buffer, establishedConnectionFD);

                                // Get the key from the client
                                recMsg(key, buffer, establishedConnectionFD);

                                // decrypt  message and send appropriate message.
                                if (decrypt (completeMessage, key) == 0)
                                {
                                    if(DEBUG){
                                        printf("Sending file\n");
                                        printStringContents(completeMessage);
                                    }
                                    sendMsg(completeMessage, establishedConnectionFD);
                                } else {
                                    char keyErr[] = "Key needs to be longer than message!"; // this shouldnt ever be reached since input val is done on client.
                                    sendMsg(keyErr, establishedConnectionFD);
                                }

                            }
                            close(establishedConnectionFD); // Close the existing socket which is connected to the client
                            if(DEBUG) printf("Child process destroyed!\n");
                            return (0);
                            break;
                        }
                default:
                         // add process to array
                         process[psize] = myPid;
                         psize++;
                         waitpid(myPid, &childExitStatus, WNOHANG);
                         //printf("background pid is %d\n", myPid);
                         fflush(stdout);
                         break;
            }
        }
    }
    close(listenSocketFD); // Close the listening socket

    return 0; 
}
