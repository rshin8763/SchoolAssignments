/*********************************************************************
 ** Program Filename:  chatclient.c
 ** Author: Ryan Shin 
 ** Date: 10/29/17
 ** Description: Project 1: Simple TCP chat client program. Designed
 ** to work with chatserve.py program. Takes turns sending messages with 
 ** server. Proper usage is chatclient SERVER_IP SERVER_PORT.
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MSG_SIZE 512
#define HANDLE_SIZE 24

#define DEBUG 0

/*********************************************************************
 ** Function Description: Prints the contents of 'buffer' in a list 
 ** by character and includes ASCII code #. Used for debugging.
 ** Pre-conditions: none
 ** Post-conditions: Contents of string is printed to stdout.
 *********************************************************************/
void printStringContents(char *buffer){
    int x = 0;
    printf("CHAR INT\n");
    for (x = 0; x < strlen(buffer); x++)
        printf (" \'%c\'  %d\n", buffer[x], buffer[x]);
    printf("---------\n");
}

/*********************************************************************
 ** Function Description: Error function used for reporting issues
 ** Pre-conditions: none
 ** Post-conditions: message is printed to stderr.
 *********************************************************************/
void error(const char *msg) { perror(msg); exit(0); }

/*********************************************************************
 ** Function Description: Gets 'handle' string from user and stores it in 'buffer.
 ** Pre-conditions: none
 ** Post-conditions: 'buffer' contains handle. It is guaranteed to be less than
 ** eleven characters.
 *********************************************************************/
void getHandle(char *buffer) {
    int i;
    char c;
    int errorFlag = 0;

    do {
        memset(buffer, '\0', sizeof(buffer)); 

        errorFlag = 0;
        printf("What would you like as your handle (one word, up to 10 characters)?\n");
        scanf("%11s", buffer);

        //flush input buffer
        while(c = getchar()!= '\n');

        //input validation
        if  (strlen(buffer) > 10 || strlen(buffer) == 0){
            printf("Please select a name between 1 and 10 characters.\n");
            errorFlag = 1;
        }
    } while (errorFlag);
    printf("Your handle is %s.\n", buffer);
    strcat(buffer, "> ");
}

/*********************************************************************
 ** Function Description: Gets string from user, prepends 'handle' string
 ** and stores it in 'msg'.
 ** Pre-conditions: Handle is 12 characters or less.
 ** Post-conditions: handle + message is stored in 'msg'. Data previously in 'msg' is
 ** overwritten, and if message == /quit" then the function returns 1. Else it
 ** returns 0. 'handle'> is written to the screen as a prompt. 'msg' will be
 ** 513 characters or less.
 *********************************************************************/
int getMessage(char *msg, char *handle) {
    int i;
    char c;
    int errorFlag = 0;
    char * temp = 0;

    memset(msg, '\0', sizeof(msg)); 

    do {
        errorFlag = 0;

        printf("%s", handle);

        size_t msgSize = 0;

        getline(&temp, &msgSize, stdin);

        temp[strcspn(temp, "\n")] = '\0';

        // return 1 if quit message was submitted
        if (strcmp(temp, "/quit") == 0){
            strcpy(msg, temp);
            free(temp);
            return 1;
        }
        if  (strlen(temp) > 500){
            printf("Message is too long (500 character maximum).\n");
            errorFlag = 1;
        } else {
            //prepend handle to message
            strcpy(msg, handle);
            strcat(msg, temp);
        }
        free(temp);
    } while (errorFlag);

    if (DEBUG) {
        printf("recieved the following input: \n");
        printStringContents(msg);
    }
    return 0;
}

/*********************************************************************
 ** Function Description: This function sends the string in 'buffer'
 ** Pre-conditions: 'buffer' contains c-style string.
 ** Post-conditions: 'buffer'is sent to 'socketFD'.
 *********************************************************************/
int sendMsg(char *buffer, int socketFD){
    int bytesSent = 0;
    int ret;

    int fileLength = strlen(buffer);

    //send file contents
    if(DEBUG){
        printf("Sending file\n");
        printStringContents(buffer);
    }
    while (bytesSent < fileLength ) {
        ret = send(socketFD, buffer + bytesSent, fileLength - bytesSent, 0);
        if (bytesSent < 0) {
            error("CLIENT: ERROR writing to socket"); 
            return -1;
        }
        else bytesSent += ret;
    }
    return 0;
}

/*********************************************************************
 ** Function Description: This function receives a string message through 'socketFD'
 ** and puts it in 'buffer', stopping when 'buffersize' bytes are read, or when
 ** recv blocks for 50ms.
 ** is recieved. Returns -1 if a '/quit/ message was received. Otherwise returns 1.
 ** Pre-conditions: none
 ** Post-conditions: 'buffer' contains the full message received before timout.
 *********************************************************************/
int recMsg(char *buffer, int buffersize, int socketFD){
    int count = 0;
    int total = 0;

    // Clear out the buffer again for reuse
    memset(buffer, '\0', buffersize); 

    //// Set up select() 
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 50000; //wait up to 50ms

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socketFD, &readfds);

    // main read loop
    while ((count = recv(socketFD, &buffer[count], buffersize, 0))>0){
        total += count;
        int retval = select(socketFD+1, &readfds, NULL, NULL, &tv);
        if (retval == 0){
            //timeout 
            count = 0;
            break;
        }
        else if (retval == -1){
            error("CLIENT: ERROR with select().");
            count = 0;
            break;
        }
    }

    if (count < 0) error("CLIENT: ERROR reading from socket");
    else if (count == 0){
        if(DEBUG){
            printf("Received file\n");
            printStringContents(buffer);
        }
        if (strcmp(buffer, "/quit") == 0) return -1;
        else return 1;
    }
}


int main(int argc, char *argv[])
{
    int socketFD, portNumber, charSent, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

    // Check usage & args
    if (argc < 3) { fprintf(stderr,"USAGE: %s server_hostname server_port\n", argv[0]); exit(0); } 

    //// Set up the server address struct

    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); 
    if(DEBUG) printf("port number: %s\n", argv[2]);

    portNumber = atoi(argv[2]);

    serverAddress.sin_family = AF_INET; 

    serverAddress.sin_port = htons(portNumber); 

    // Convert the machine name into a special form of address
    serverHostInfo = gethostbyname(argv[1]); 
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }

    // Copy in the address
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); 

    //// Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0) error("CLIENT: ERROR opening socket");


    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ 
        fprintf(stderr, "Error: could not contact server '%s' on port %s\n", argv[1], argv[2]);
        exit(2);
    }

    //assign buffers
    char handle[HANDLE_SIZE];
    char msg[MSG_SIZE];

    // Assign handle
    getHandle(handle);

    //// Main loop
    while (1){
        int ret = getMessage(msg, handle);
        // if quit command is inputted
        if (ret == 1) {
            sendMsg(msg, socketFD);
            printf("Closed connection with server.\n");
            break;
        }

        // Send message to server
        sendMsg(msg, socketFD);

        // Get return message from server
        char response[MSG_SIZE];
        ret = recMsg(response, MSG_SIZE, socketFD);
        if (ret == 1) printf("%s\n", response);
        else {
            printf("Server closed the connection.\n");
            break;
        }
    }
    close(socketFD);
    return 0;
}

