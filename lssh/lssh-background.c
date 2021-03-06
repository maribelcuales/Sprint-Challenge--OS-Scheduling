#include <stdio.h>  // install the necessary libraries
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>  // to print error message for chdir() system call     
// #include <sys/wait.h>   // to call on the wait function

#define PROMPT "lambda-shell$ "

#define MAX_TOKENS 100
#define COMMANDLINE_BUFSIZE 1024
#define DEBUG 1  // Set to 1 to turn on some debugging output, or 0 to turn off

/**
 * Parse the command line.
 *
 * YOU DON'T NEED TO MODIFY THIS!
 * (But you should study it to see how it works)
 *
 * Takes a string like "ls -la .." and breaks it down into an array of pointers
 * to strings like this:
 *
 *   args[0] ---> "ls"
 *   args[1] ---> "-la"
 *   args[2] ---> ".."
 *   args[3] ---> NULL (NULL is a pointer to address 0)
 *
 * @param str {char *} Pointer to the complete command line string.
 * @param args {char **} Pointer to an array of strings. This will hold the result.
 * @param args_count {int *} Pointer to an int that will hold the final args count.
 *
 * @returns A copy of args for convenience.
 */
char **parse_commandline(char *str, char **args, int *args_count)
{
    char *token;
    
    *args_count = 0;

    token = strtok(str, " \t\n\r"); // strtok breaks a string into a series of tokens/substrings passing in a delim; like str.split in JS

    while (token != NULL && *args_count < MAX_TOKENS - 1) { // while loop
        args[(*args_count)++] = token; // keeps iterating over the args array if token has not yet reached null 
        token = strtok(NULL, " \t\n\r");    
    }

    args[*args_count] = NULL;   // Null means having reached the end of the args array

    return args;
}

/**
 * Main
 */
int main(void)
{
    // Holds the command line the user types in
    char commandline[COMMANDLINE_BUFSIZE];  // entire string that the user types in

    // Holds the parsed version of the command line
    char *args[MAX_TOKENS]; // initializing buffers

    // How many command line args the user typed
    int args_count;

    // Shell loops forever (until we tell it to exit)
    while (1) {
        // Print a prompt
        printf("%s", PROMPT);
        fflush(stdout); // Force the line above to print

        // Read input from keyboard
        fgets(commandline, sizeof commandline, stdin);

        // Exit the shell on End-Of-File (CRTL-D)
        if (feof(stdin)) {  // checking for ctrl D which can exit shell
            break;
        }

        // Parse input into individual arguments
        parse_commandline(commandline, args, &args_count);  // taking in the string input by the user, tokes in args array, and the array count

        if (args_count == 0) {
            // If the user entered no commands, do nothing
            continue;
        }

        // Exit the shell if args[0] is the built-in "exit" command
        if (strcmp(args[0], "exit") == 0) {
            break;
            // exit(1);
        }

        // implement "cd" command to change directories
        if (strcmp(args[0], "cd") == 0) // strcmp compares the string in the first argument args[0] with the string "cd"
            // grab the path the user specified
            // ensure that user supplied the path
        { 
            if (args_count != 2 ) //error handling; if arguments passed are not equal to 2
            {  
                fprintf(stderr, "Error. Please enter correct directory format: cd <directory_name>\n"); // print this error message
                continue; // continues the loop/ iteration
            }

            // Run the chdir() system call on the second argument args[1] to change directories
            if (chdir(args[1]) < 0) // error handling; if second argument is less than 0 or -1  
            {
                perror("chdir");    // print this error message
                continue;           // continues the loop/ iteration
            }
            continue;               // continues the loop/ iteration
        }

        int bg = 0; // initialize bg

        // check for the & at the end of the args array
        if (strcmp(args[args_count-1], "&") == 0) {
            // flip a boolean to indicate background task should be enabled
            bg = 1; // for true
            // strip away the &
            args[args_count-1] = NULL;  // set to null 
        }

        while (wait(NULL) > 0); // while loop

        #if DEBUG

        // Some debugging output

        // Print out the parsed command line in args[]
        for (int i = 0; args[i] != NULL; i++) {
            printf("%d: '%s'\n", i, args[i]);
        }

        #endif
        
        /* Add your code for implementing the shell's logic here */

        //Fork a child process to run the new command
        pid_t child_pid = fork(); // initialize child_pid and set it to the fork system call; pid_t is a data type for representing a process ID 

        // error handling
        if (child_pid < 0)  // if fork failed 
        {
            fprintf(stderr, "Fork failed.\n");   // if fork failed, print error message
            continue;    // continues the loop/ iteration
        }

        // Exec the command in the child process
        // Exec functions replaces the current process with the new process called by exec
        // This means that the child process can run a program different from what the parent is running
        if (child_pid == 0) // if fork successful; in child process context 
        {
            execvp(args[0], args); // calls the exec function and runs the program for the child process taking in args
            fprintf(stderr, "Function Exec failed to run.\n");   // print in case of error            
            // continue;   // continues the loop/ iteration
            exit(1);
        } else {
            // in the parent context 
            // check to see if the bg boolean is flipped to true
            if (!bg) {  // if bg is false then we'll wait
                wait(NULL); // Parent process waits for child to complete
            }            
        }        
    }

    return 0;
}