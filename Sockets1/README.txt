README

Description: Project 1- Simple chat server and client using TCP
Author: Ryan Shin

HOW TO USE

First, compile chatclient.c with the following command (within the same directory as the file):

----------------------------------
gcc chatclient.c -o chatclient
----------------------------------

Then, run chatserve.py using python 3 with the following command:

----------------------------------
python3 chatserve.py PORTNUM
----------------------------------

with any port number of your choosing. Make sure the file has executable permissions.
If not, use chmod +x chatserve.py.

The python script should say "Listening on IP : PORTNO."
Finally run chatclient with the following:

----------------------------------
chatclient HOSTNAME PORTNUM
----------------------------------

Use the port number you chose earlier, and use the host name of the host the server is running on.
You may use the IP address the python script printed when it was started.

The client program prompts the user for a handler (10 characters or less). Afterwards, the client and host take
turns sending messages. The connection is closed if either type '/quit.' After the connection is closed, the server
blocks, listening on the port until another client connects. The only way to stop the server program is by
sending a signal such as SIGINT or SIGSTOP. 
