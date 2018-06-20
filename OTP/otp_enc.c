// Author: Ryan Shin
// 8/18/17
// CS344
// Assignment 4- OTP
// File: otp_enc.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define BUFF_SIZE 100000
#define DEBUG 0

// Function that prints the ASCII numbers for characters in a String. Used for debugging.
void printStringContents(char *buffer){
    int x = 0;
    printf("CHAR INT\n");
    for (x = 0; x < strlen(buffer); x++)
        printf (" \'%c\'  %d\n", buffer[x], buffer[x]);
    printf("---------\n");
}

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

// Takes filename and buffer strings and writes all data from file at filename into buffer.
// Checks for non capital alphabet letters excluding spaces and returns 1 if there was an error
// with the file. (Either the file cannot be read or contains bad characters)
int writeFileToString(char *filename, char *buffer){
    if(DEBUG) printf("Opening file %s\n", filename);

    FILE *fp = fopen(filename, "r"); // open file for reading
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open %s for reading\n", filename);
        fflush(stderr);
        return 1;
    }

    char c = ' '; //dummy value
    int i = 0;

    //get characters from file
    while(c != EOF){
        c = getc(fp);
        buffer[i] = c;
        i++;
    }

    fclose(fp);

    //remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    //check for bad characters
    for (i=0; i<strlen(buffer); i++){
        if (buffer[i] != ' ' && (buffer[i] < 'A' || buffer[i] > 'Z')){
            fprintf(stderr, "Error: '%s' contains bad characters\n", filename);
            return 1;
        }
    }
    return 0;
}

// This fucntion sends the string in buffer ending with a terminal character '@@'
// to the socket specified in socketFD. 
int sendMsg(char *buffer, int socketFD){
    int bytesSent = 0;
    int ret;
    //add terminal characters
    strcat(buffer, "@@");

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

// This fucntion receives a string message through the socketFD,
// and constructs a string completeMessage, stopping when a terminal character '@@'
// is received. 'buffer' will contain a part of that message after the function terminates.
void recMsg(char *completeMessage, char *buffer, int socketFD){
    int charsRead = 0;
    memset(completeMessage, '\0', sizeof(completeMessage)); // Clear out the buffer again for reuse
    while (strstr(completeMessage, "@@") == NULL){
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
        strcat(completeMessage, buffer);
        if (charsRead < 0) error("CLIENT: ERROR reading from socket");
    }
    //remove control characters
    completeMessage[strcspn(completeMessage, "@")] = '\0';
}

int main(int argc, char *argv[])
{
    int socketFD, portNumber, charSent, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[1028];

    if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    if(DEBUG) printf("port number: %s\n", argv[3]);
    portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address


    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");

    //assign buffers
    char msg[BUFF_SIZE];
    char key[BUFF_SIZE];

    memset(msg, '\0', BUFF_SIZE);
    memset(key, '\0', BUFF_SIZE);

    //fill buffers with file content and do input validation
    int flag = 0;
    flag += writeFileToString(argv[1], msg);
    if (flag >= 1){
        exit(1);
    }
    flag += writeFileToString(argv[2], key);
    if (flag >= 1){
        exit(1);
    }

    // Check to see if key is long enough
    if (strlen(key) < strlen(msg)){
        fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
        exit(1);
    }

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
        fprintf(stderr, "Error: could not contact otp_dec_p on port %s\n", argv[3]);
        exit(2);
    }

    charSent = 0;

    //create authentication code.
    char authcode[] = "enc";
    charSent = send(socketFD, authcode, sizeof(authcode), 0);
    if (charSent < 0) {error("CLIENT: ERROR writing to socket");}
    if (charSent < strlen(authcode)) error("CLIENT: WARNING: Not all data written to socket!\n");

    //receive message
    recMsg(authcode, buffer, socketFD);

    // if authentication passes, send data
    if (strcmp(authcode, "yes") == 0){
        // Send message to server
        sendMsg(msg, socketFD);

        // Send key to server
        sendMsg(key, socketFD);

        // Get return message from server
        char completeMessage[BUFF_SIZE];
        recMsg(completeMessage, buffer, socketFD);

        //output encrypted string to stdout
        printf("%s\n", completeMessage);
        // if authentication fails, return the following error as per the example and exit with value 2.
    } else {
        fprintf(stderr, "Rejected by otp_dec_d on port %s (you must use otp_dec with otp_dec_d)\n", argv[3]);
        close(socketFD);
        exit(2);
    }

    //Close the socket
    close(socketFD);
    return 0;
}

