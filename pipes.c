// Assignment 8:    Redirect Streams (small)
// Author:          Angel Penaloza
// Description:     Handling commands. 
//                  Format would be <input> <command> <output>
//                  Input and output are file names.                  

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


const char* IN_FILE = "input.txt";
const char* OUT_FILE = "output.txt";

int main(int argc, char* argv[]) {

    // if(argc < 4) {          // make sure there are exactly 3 inputs
    //    printf("invalid number of inputs: %d\n", argc);
    //    exit(1); 
    // }

    char* inp = argv[1];
    char* cmd = argv[2];
    char* out = argv[3];

    // split words on whitespace
    char *cmd_copy = strdup(cmd);
    char *args[10];
    int ix = 0;
    char *token = strtok(cmd_copy, " ");
    while (token != NULL) {
        args[ix++] = token;
        token = strtok(NULL, " ");
    }
    args[ix] = NULL; 

    // find absolute path
    char command_path[100];
    if (strchr(args[0], '/') == NULL) {
        char *path_env = getenv("PATH");
        if (path_env) {
            char *path = strtok(path_env, ":");
            while (path) {
                snprintf(command_path, sizeof(command_path), "%s/%s", path, args[0]);
                if (access(command_path, X_OK) == 0) {
                    args[0] = command_path;
                    break;
                }
                path = strtok(NULL, ":");
            }
        }
        if (!path_env || access(args[0], X_OK) != 0) {
            fprintf(stderr, "invalid command\n");
            exit(1);
        }
    }

    int file_desc[2];
    pipe(file_desc);
    pid_t pid = fork();

    // child
    if(pid == 0) {
        // open input file
        file_desc[0] = open(IN_FILE, O_RDONLY);
        if(file_desc[0] < 0) {
            printf("error opening file\n");
            exit(1);
        }
        dup2(file_desc[0], STDIN_FILENO);

        // open output file
        file_desc[1] = open(OUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(file_desc[1] < 0) {
            printf("error opening file\n");
            exit(1);
        }
        dup2(file_desc[1], STDERR_FILENO);

        // execute command
        if(execvp(argv[0], args) < 0) exit(0);

    } 
    
    // parent
    else if(pid > 0) {
        int status;
        waitpid(pid, &status, 0);

        if(file_desc[0] != -1) close(file_desc[0]);
        if(file_desc[1] != -1) close(file_desc[1]);
        
        if(WIFEXITED(status)) printf("exitied with status %d\n", WEXITSTATUS(status));
        else printf("error exiting\n");
    }

    // invalid 
    else {
        printf("error creating fork\n");
        exit(1);
    }

    return 0;
}