#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h> 

#define MAX_ARG 25

int main (int argc, char *argv[]){
    char input[MAX_ARG];
    while(1){
        printf("lab1> ");
        //get the input from the user
        fgets(input, MAX_ARG, stdin);
        //strips newline in input
        // len = strlen(str);
        // if( str[len-1] == '\n' )
        //     str[len-1] = 0;
        input[strlen(input) - 1] = '\0';

        printf("Parent Process %d\n", getpid());
        //the string entered by the user is a built-in command
        if(strcmp(input, "printid") == 0){
            //printid : print on the screen The ID of the current process is X where x is the process ID
            printf("The ID of the current process is %d\n", getpid());
        }else if(strcmp(input, "exit") == 0){
            //exit : ends the program and exits
            exit(0);
        }else if(strcmp(input, "greet") == 0){
            //greet: prints “Hello\n” on the screen
            printf("Hello\n");
        }else{
            int pid = fork();
            if(pid == 0){ //the created child process
                printf("Child process %d will execute the command %s\n", getpid(), input); //print ID of the child process and the string entered by the user
                // You will need to add the /bin/ to the user input and use that as the first argument for execve()
                char command[MAX_ARG] = "/bin/";
                //concats the '/bin/' with the command the user inputed to make the proper command for execve
                strcat(command, input); 

                // the second argument is a pointer to an array of string
                // Example: Suppose the user inputs: ls Then the inputs to execve will be: 
                // execve(“/bin/ls”, progname, NULL); Where progname was declared as: 
                char * progname[]= {input, NULL};

                //execve() use the string entered by the user to transform the child process to the new process.
                execve(command, progname, NULL);
                //if execution of the child process reaches this point, it means execve failed. In that case, print “Command Not Found!\n” and exits.
                printf("Command Not Found!\n");
                exit(0);
            }
        }
        //wait till the child process finishes
        wait(NULL);
    }
}

//FOR OWN TESTING
// /bin/ls : lists the files in the current directory
// /bin/pwd : prints the path of the current working directory
// /bin/ps : prints the status of the current processes
// /bin/date : prints current date and time
// /bin/lscpu: prints information about the current processes of the machine
