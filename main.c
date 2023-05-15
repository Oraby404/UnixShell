#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

static FILE *fp;

void signal_handler(){
    fp = fopen("logfile.txt","a");
    fprintf(fp,"Child Process Was Terminated!\n");
    fclose(fp);
}
void execute(char *input, char *command, char argument_indicator) {

    bool wait_indicator = true;
    //prepare the arguments
    int i = 0;
    char **args = calloc(2,sizeof(char *));
    args[i++] = command;

    if (argument_indicator) {//an input with arguments (needs tokenization)
        char *token = strtok(input, " ");
        while (token != NULL) {
            token = strtok(NULL, " ");
            args = (char **) realloc(args, sizeof(char *) * (i + 2));
            args[i++] = token;
        }
        if (!strcmp(args[i - 2], "&")) {
            wait_indicator = false;
            args[i - 2] = NULL;
        }
    }

    signal(SIGCHLD,signal_handler);
    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork Failed to Create Child Process!\n");
    } else if (!pid) {//child
        execvp(command, args);
        printf("Unsupported Command or Argument!\n");
        exit(0);
    } else {//parent
        if (wait_indicator)
            waitpid(pid, NULL, 0);
    }
}

int main() {

    //create a new file
    fp = fopen("logfile.txt","w");
    fclose(fp);

    char user_input[255];//the input line from user
    char user_command[10];//the command in user input

    bool running = true;

    while (running) {

        char arg_indicator = 0;
        printf("Shell >");

        fgets(user_input, 255, stdin);
        user_input[strlen(user_input) - 1] = '\0';//replace \n with null

        //remove leading and tailing whitespaces (no need to tokenize for command with no arguments)
        sscanf(user_input, "%s %c", user_command, &arg_indicator);//get the command and argument indication

        if (!strcasecmp(user_command, "exit"))
            running = false;
        else {
            execute(user_input, user_command, arg_indicator);
        }

    }

    printf("Returning to Main Shell.\n");
    return 0;
}


