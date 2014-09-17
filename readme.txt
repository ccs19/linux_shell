//Very loose readme.

Authors: 
Christopher Schneider
Branden Sherrell


Description:
Project 1 for Sysnet 1.  A Linux shell.

Instructions:
Compile code with make.  Execute the resulting myshell file.

Assumptions:
-The vector containing the arguments shall be only 32 indicies in length. As such, only the system command name
and 31 additional parameters (excluding file redirection and backgrounding requests) are supported. Note that
input such as "ls -la -xyZ" represent a total of 3 indices (command name and 2 sets of passed parameters).

-Child stdout and stdin redirection is not handled unless specified. As such, since the child process inherits
the file-descriptor table from the parent, any output or input associated with the child process will either be
written to the stdout or taken from the stdin, respectively. 