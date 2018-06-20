README

Description: Project 2- Simple file transfer server and client using TCP
Author: Ryan Shin

HOW TO USE

First, compile fserver.c with the following command (within the same directory as the file):

----------------------------------
gcc fserver.c -o fserver
----------------------------------

Then, run fserver with the following command:

----------------------------------
fserver PORTNUM
----------------------------------

Let PORTNUM be any number between 1024 and 65535, but try to make it unique.


Then, run fclient.py using python 3 with either of the following commands depending on if you want to request a directory listing (-l) or get a file (-g):

--------------------------------------------------------
python3 fclient.py HOSTNAME PORTNUM -l DATAPORT
--------------------------------------------------------
--------------------------------------------------------
python3 fclient.py HOSTNAME PORTNUM -g FILENAME DATAPORT
--------------------------------------------------------
where PORTNUM is the same number that you set for the server.
Choose any viable port number for DATAPORT. It should not be the same port as PORTNUM. If you get a message saying that the port is used, try a different one.

Make sure the python has executable permissions.
If not, use chmod +x chatserve.py.

You should first try the -l command to get a directory listing, then choose any .txt file listed and input the -g command with that filename in the correct command line argument. The client will prompt you if the transferred file needs to renamed due to a naming collision.

After each closed connection, the server will block, listening on the port until another client connects. The only way to stop the server program is by sending a signal such as SIGINT or SIGSTOP. 
