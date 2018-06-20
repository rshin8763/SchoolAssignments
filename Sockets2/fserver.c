/*********************************************************************
 ** Program Filename:  fserver.c
 ** Author: Ryan Shin 
 ** Date: 11/26/17
 ** Description: This is the server program for a simple file transfer system.
 ** References: Beej's guide to Networking was used as a general reference.
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <dirent.h>

#define DEBUG 0
#define BUFF_SIZE 2048

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
 ** Function Description: This function was taken from Beej's network
 ** guide, section 5.3. This function prevents recently free'd ports
 ** from giving an error.
 *********************************************************************/
int freePorts(int socketFD){
    int yes=1;

    if (setsockopt(socketFD,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    } 
}

/*********************************************************************
 ** Function Description: Prints the contents of the current working directory
 ** to the passed buffer.
 ** Pre-conditions: none
 ** Post-conditions: Buffer contains the files and folders of the cwd
 ** separated by newlines.
 *********************************************************************/
void listDir(char *buffer){
    DIR *dirp = opendir(".");
    struct dirent *dp;
    char temp[BUFF_SIZE];
    memset(temp, '\0', sizeof(temp));
    memset(buffer, '\0', sizeof(buffer));
    while ((dp = readdir(dirp)) != NULL){
        sprintf(temp, "%s\n", dp->d_name);   
        strcat(buffer, temp);
    }
    closedir(dirp);
}
/*********************************************************************
 ** Function Description: This function sends the string in 'buffer' to
 ** the socket indicated by socketFD.
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
        printf("Filelength: %d\n", fileLength);
    }

    while (bytesSent < fileLength ) {
        ret = send(socketFD, buffer + bytesSent, fileLength - bytesSent, 0);
        if (bytesSent < 0) {
            perror("CLIENT: ERROR writing to socket"); 
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
 ** Pre-conditions: none
 ** Post-conditions: 'buffer' contains the full message received before timout.
 ** Returns 1 if receive is successful. Otherwise, returns -1.
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
            perror("CLIENT: ERROR with select().");
            count = 0;
            return -1;
        }
    }
    if (count < 0){
        perror("CLIENT: ERROR reading from socket");
        return -1;
    } else if (count == 0){
        if(DEBUG){
            printf("Received file\n");
            printStringContents(buffer);
        }
        return 1;
    }
}

/*********************************************************************
 ** Function Description: This function writes the contents of a file
 ** filename in the cwd to a string buffer pointed by buffer.
 ** pre-conditions: 'buffer' should be big enough to contain the file.
 ** buffer must also be memset to filled with 0's.
 ** Post-conditions: 'buffer contains the file.
 *********************************************************************/
int writeFileToString(char *filename, char *buffer){
    if(DEBUG) printf("Opening file %s\n", filename);
    // open file for reading
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return -1;
    }

    char c; 
    int i = 0;

    while(1){
        c = getc(fp);
        if (c == EOF) break;
        buffer[i] = c;
        i++;
    }

    fclose(fp);
    return 0;
}

/*********************************************************************
 ** Function Description: Sets up a TCP socket and binds it to 'portNo'
 ** Also sets the program to listen on the socket for up to 5 connections.
 ** Pre-conditions: none
 ** Post-conditions: A socket file descriptor is returned for the active socket.
 *********************************************************************/
int setupSocket(int portNo){
    struct sockaddr_in serverAddress;

    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(portNo); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 

    // Set up the socket
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        perror("ERROR opening socket");
        return -1;
    }

    freePorts(socketFD);

    // Enable the socket to begin listening
    if (bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("ERROR on binding");
        return -1;
    }

    // Flip the socket on - it can now receive up to 5 connections
    if (listen(socketFD, 5) < 0){
        perror("ERROR on listen");
        return -1;
    } else {
        if (DEBUG){
            printf("Listening on port %d:\n", portNo);
        }
    }

    return socketFD;
}

/*********************************************************************
 ** Function Description: This function handles the various requests
 ** that the client sends. The function validates the requests and sends 
 ** back the appropriate responses. 
 *********************************************************************/
int handleRequest(char* request, int controlConnectionFD){
    int dataPortNo;

    //Parse commands
    int i = 0;
    char commands[3][BUFF_SIZE];

    for (i = 0; i < 3; i++) {
        memset(commands[i], '\0', BUFF_SIZE);
    }

    char *token = strtok(request, " ");
    strcpy(commands[0], token);

    token = strtok(NULL, " ");
    for(i = 1; i < 3; i++){
        strcpy(commands[i], token);
        token = strtok(NULL, " ") ;
        if (token == NULL) break;
    }

    if (DEBUG){
        for (i = 0; i < 3; i++) {
            printf(" %s\n", commands[i]);
        }
    }
    //

    // Handle list request
    if (strcmp(commands[0],"-l" ) == 0){
        dataPortNo = atoi(commands[1]);
        if (dataPortNo == 0) {
            printf("Received an invalid port number.");
            //Send Error Message
            sendMsg("Occupied or Invalid Port", controlConnectionFD);
        } else{

            // Setup data connection socket
            int dataSocket =  setupSocket(dataPortNo);
            if (dataSocket < 0){
                sendMsg("Invalid Data Port", controlConnectionFD);
            } else{

                printf("List Directory requested on port %d\n", dataPortNo);

                // Send msg to control connection approving data connection
                sendMsg("Yes", controlConnectionFD);

                listDir(request);

                int dataConnectionFD = accept(dataSocket, NULL, NULL);

                printf("Sending directory contents to client\n");
                sendMsg(request, dataConnectionFD);

                close(dataConnectionFD);
                close(dataSocket);
            }
        }

        // Handle get request
    } else if (strcmp(commands[0],"-g") == 0){
        dataPortNo = atoi(commands[2]);

        if (dataPortNo == 0) {
            printf("Received an invalid port number.");
            //Send Error Message
            sendMsg("Invalid Port", controlConnectionFD);
        }
        else{
            printf("File \"%s\" requested on port %d\n", commands[1], dataPortNo);

            // Setup data connection socket
            int dataSocket =  setupSocket(dataPortNo);
            if (dataSocket < 0){
                sendMsg("Invalid Data Port", controlConnectionFD);
            } else {
                int flag;
                // open file for reading
                FILE *fp = fopen(commands[1], "r");
                if (fp == NULL) {
                    flag = -1;
                }

                if (flag == -1) {
                    printf("File not found. Sending error message to client\n");
                    //Send Error Message
                    sendMsg("FILE NOT FOUND", controlConnectionFD);
                } else {
                    // Send msg to control connection approving data connection
                    sendMsg("Yes", controlConnectionFD);

                    // get file size
                    fseek(fp, 0L, SEEK_END);
                    int size = ftell(fp);
                    rewind(fp);

                    if (DEBUG){ printf("size is %d.\n", size); }

                    char file[size+1];
                    memset(file, 0, size+1);

                    writeFileToString(commands[1], file);

                    printf("Sending \"%s\" contents to client\n", commands[1]);
                    int dataConnectionFD = accept(dataSocket, NULL, NULL);

                    sendMsg(file, dataConnectionFD);

                    close(dataConnectionFD);
                    close(dataSocket);
                }
            }
        }
    } else {
        printf("Client sent invalid command.");
        //Send Error Message
        sendMsg("Invalid command", controlConnectionFD);
    }
}

void main(int argc, char *argv[]){
    int socketFD, dataPortNo, controlPortNo;
    struct sockaddr_in clientAddress;
    socklen_t sizeOfClientInfo;
    sizeOfClientInfo = sizeof(clientAddress);

    //Check usage and arguments
    if(argc != 2){
        printf("USAGE: fserver [PORTNO]\n");
    } else {
        controlPortNo = atoi(argv[1]); // Get the port number, convert to an integer from a string
        // Startup
        socketFD = setupSocket(controlPortNo);
        printf("Server open on %d\n", controlPortNo);
        if (socketFD >= 0){
            // Main server loop
            while (1){
                // Accept a control connection
                int controlConnectionFD = accept(socketFD,(struct sockaddr *)&clientAddress,&sizeOfClientInfo); 
                if (controlConnectionFD < 0) perror("ERROR on accept");

                // Get client info
                struct hostent *client = gethostbyaddr(&clientAddress.sin_addr, sizeof(clientAddress.sin_addr), AF_INET);
                printf("Connection from %s\n", client->h_name);

                char request[BUFF_SIZE];

                // Receive request
                recMsg(request, BUFF_SIZE, controlConnectionFD);

                // Handle request
                handleRequest(request, controlConnectionFD);

                // Close connection
                close(controlConnectionFD);
            }
        }
        // Close the listening socket
        close(socketFD); 
    }
    return; 
}
