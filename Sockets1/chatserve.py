
## Program Filename:  chatserve.py
## Author: Ryan Shin 
## Date: 10/29/17
## Description: Simple TCP chat server program designed to work with chatclient.c.
## The chat and server take turns sending messages until '/quit' is entered. This ends 
## the current client-server connection, but the server will remain active and listening
## until SIGINT is received.

import sys
import socket

## Function Description: Gets string from user, prepends 'handle' to it
## and returns the concatenated string.
## Pre-conditions: 'handle' is a string.
## Post-conditions: 'handle'> is written to the screen as a prompt.
## The user input will be less than or equal to 500 characters. If 
## '/quit' was entered, then the function returns '/quit/', else
## it returns 'handle' + 'msg' 

def getMessage(handle):

    while True:

        ## Get input
        msg = input(handle)

        ## If it is quit message, exit loop.
        if msg == "/quit":
            break

        ## validate input
        if  len(msg) > 500:
            print ("Message is too long (500 character maximum).")
            
        ## if valid, prepend handle to message
        else:
            msg = handle + msg
            break
            
    return msg


def main ():
    ## Show usage error message if number of arguments is incorrect
    if len(sys.argv) != 2:
        print ("USAGE: chatserve PORTNO")

    else: 
        ## Set up the socket
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ip = socket.gethostbyname(socket.gethostname())
        port = int(sys.argv[1])
        address=(ip,port)
        server.bind(address)

        ## Enable the socket to begin listening on port
        server.listen(1)

        print ("Started listening on", ip, ":", str(port))

        ## Hard-coded Server 'handle'
        handle = "Server> "

        ## Main server loop
        while True:
            ## Accept connection request
            client, addr = server.accept()
            print ("Got a connection from",addr[0],":",addr[1])

            ## Chat loop
            while True:
                ## Get the message from the client and display it
                response = client.recv(1024).decode()
                print (response)

                ## if quit command was received from client
                if response == "/quit":
                    break

                ## Get message from user
                msg = getMessage(handle)

                ## if quit command was inputted
                if msg == "/quit":
                    client.send(msg.encode())
                    break

                ## Send message to server
                client.send(msg.encode())

            ## Close the existing socket which is connected to the client
            client.close()
            print ("Closed connection with client.")

        ## Close the listening socket
        server.close

if __name__ == "__main__":
    main()

