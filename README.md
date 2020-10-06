# Shell
Command line Shell written in C.
Commands:

"movetodir PATH" - It is used to change the current directory (which must be an internal variable) to the specified one.

"whereami" - prints the current directory

"history [-c]" - Without the parameter, prints out the recently typed commands (with their arguments). If “-c” is passed, it clears the list of recently typed commands 

"byebye" - terminates the shell.

"start program [parameters]" - executes the program with the optional parameters.

"background program [parameters]" - It is similar to the run command, but it immediately prints the PID of the program it started, and returns the prompt.

"exterminate PID" - Immediately terminate the program with the specific PID (presumably started from this command line interpreter).
