## Program Filename:  fclient.py
## Author: Ryan Shin 
## Date: 11/26/17
## Description: This is a simple TCP file transfer client program designed 
## to work with fserver.c. Use with Python3. 
## (E.x.) python3 fclient.py {servername} {controlPort} {-g/-l} [fileName] {dataPort}

import sys
import socket
import time
import os.path

## Function:  connectSocket()
## Description: This function creates a TCP socket to connect to
## passed hostname and port number. 

def connectSocket(hostname, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((hostname, port))
    print ("Connecting to server at " + hostname + ":" + str(port))
    return s



## Class to contain command line arguments
class Command(object):
    def __init__(self, serverName, controlPort, command, fileName, dataPort):
        self.serverName = serverName
        self.controlPort = controlPort
        self.command = command
        self.fileName = fileName
        self.dataPort = dataPort

## Function: validateCommand()
## Description: Validates the command line arguments to match usage cases.
def validateCommand():
    if (len(sys.argv) < 5) or (len(sys.argv) > 6):
        print ("USAGE: chatclient [SERVER_NAME] [CONTROL_PORT] [COMMANDS] [DATA_PORT]")

    else: 
        ## validate input
        serverName = sys.argv[1]
        controlPort = int(sys.argv[2])
        command = sys.argv[3]

        if command == "-l":
            if (len(sys.argv)!= 5):
                print ("USAGE: chatclient [SERVER_NAME] [CONTROL_PORT] [-l] [DATA_PORT]")
                return None
            else:
                dataPort = int(sys.argv[4])
                if dataPort < 1024 or dataPort > 65535:
                    print("Pick a port number inside 1024 and 65535 (larger is better).")
                    return None
                return Command(serverName, controlPort, command, 0, dataPort)
        elif command == "-g":
            if (len(sys.argv)!= 6):
                print ("USAGE: chatclient [SERVER_NAME] [CONTROL_PORT] [-g] [FILENAME] [DATA_PORT]")
                return None
            else:
                fileName = sys.argv[4]
                dataPort = int(sys.argv[5])
                if dataPort < 1024 or dataPort > 65535:
                    print("Pick a port number inside 1024 and 65535 (larger is better).")
                    return None
                return Command(serverName, controlPort, command, fileName, dataPort)
        else :
            print ("USAGE: chatclient [SERVER_NAME] [CONTROL_PORT] [COMMANDS] [DATA_PORT]")
            return None

## Function: makeRequest()
## Description: This function makes the request contained in the passed command 
## object to the socket indicated by controlSocket.
def makeRequest(command, controlSocket):
    if command.command == "-l":
        req =  command.command + " " + str(command.dataPort)
        controlSocket.send(req.encode())
    else:
        req = command.command + " " + command.fileName + " " + str(command.dataPort)
        controlSocket.send(req.encode())

## Function: receiveFile()
## Description: This function receives a file from a socket
## and writes it to the current working directory. It uses the 
## the filename of the file received. If there is a naming conflict,
## it prompts the user to input a new name.
def receiveFile(command, dataSocket):
    response = ""
    while (True):
        temp = dataSocket.recv(1024).decode()
        if temp == "":
            break
        response += str(temp)
    print ("File transfer complete.")
    fileName = command.fileName
    while (os.path.isfile(fileName)):
        print ("\"" + fileName + "\" already exists in the directory!")
        fileName = input("Please type a new name for the transfered file: ") 
    file = open(fileName, "w")
    file.write(response)
    print("\""+fileName+"\" has been created.") 
    

## Function: handleResponse()
## Description: This function handles the responses received by the server.
## This function should be called immediately after makeRequest(). This function
## either creates connects a data socket to the dataPort member of the command object
## and receives the server's response, or it recieves an error message from the server
## and outputs it to the user.
def handleResponse(command, controlSocket):
    response = controlSocket.recv(1024).decode()
    if response == "Yes":
        ## set up data socket
        data = connectSocket(command.serverName, command.dataPort)

        if command.command == "-l":
            response = data.recv(1024).decode()
            print ("Receiving directory structure from "+command.serverName+":"+str(command.dataPort))
            print (response)
        elif command.command == "-g":
            print ("Receiving \""+command.fileName+"\" from "+command.serverName+":"+str(command.dataPort))
            receiveFile(command, data)

        ## close data socket
        data.close()

    else:
        print (command.serverName+":"+str(command.controlPort),"says",response)


def main ():
    ## Show usage error message if number of arguments is incorrect
    val = validateCommand()
    if val == None:
        ## Commmand is invalid.
        return
    else:
        ## set up the socket
        control = connectSocket(val.serverName, val.controlPort)

        makeRequest(val, control)

        handleResponse(val, control)

        ## close the socket
        control.close()
        print ("Closed connection with Server:" , val.serverName)


if __name__ == "__main__":
    main()

