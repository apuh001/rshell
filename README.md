# rshell
Rshell assignment for CS100

apuh001 

My project basically relays user input into execvp for execution within a child process.
I wrote my own tokenization function that uses Boost's split function to tokenize the
user input. My prompt function gets input from user and forks the process and calls
execvp. 

Known bugs:
Echo echoes the quotations. Shouldn't do that.

