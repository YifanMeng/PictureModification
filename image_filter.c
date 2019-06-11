#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "bitmap.h"


#define ERROR_MESSAGE "Warning: one or more filter had an error, so the output image may not be correct.\n"
#define SUCCESS_MESSAGE "Image transformed successfully!\n"


/*
 * Check whether the given command is a valid image filter, and if so,
 * run the process.
 *
 * We've given you this function to illustrate the expected command-line
 * arguments for image_filter. No further error-checking is required for
 * the child processes.
 */
void run_command(const char *cmd) {
    if (strcmp(cmd, "copy") == 0 || strcmp(cmd, "./copy") == 0 ||
        strcmp(cmd, "greyscale") == 0 || strcmp(cmd, "./greyscale") == 0 ||
        strcmp(cmd, "gaussian_blur") == 0 || strcmp(cmd, "./gaussian_blur") == 0 ||
        strcmp(cmd, "edge_detection") == 0 || strcmp(cmd, "./edge_detection") == 0) {
        execl(cmd, cmd, NULL);
    } else if (strncmp(cmd, "scale", 5) == 0) {
        // Note: the numeric argument starts at cmd[6]
        execl("scale", "scale", cmd + 6, NULL);
    } else if (strncmp(cmd, "./scale", 7) == 0) {
        // Note: the numeric argument starts at cmd[8]
        execl("./scale", "./scale", cmd + 8, NULL);
    } else {
        fprintf(stderr, "Invalid command '%s'\n", cmd);
        exit(1);
    }
}


// TODO: Complete this function.
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: image_filter input output [filter ...]\n");
        exit(1);
    }
    int child_count = argc - 3;
    char **children = argv + 3;

    //If no filters are provided, perform a single copy filter    
    if (child_count == 0) {
        child_count = 1;
        char *copy = "copy";
        children = &copy;
    }

    int pipe_fd[child_count][2];
    int i;
    
    for (i = 0; i < child_count; i++) {
        //Create the pipe
        if (pipe(pipe_fd[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        int result = fork();
        if (result < 0) {
            perror("fork");
            exit(1);

        //Child process
        } else if (result == 0) {

            if (i == 0) { //The first child
                //Set stdin to read from the input file
                int f1 = open(argv[1], O_RDONLY);
                if (dup2(f1, fileno(stdin)) == -1) {
                    perror("dup2 input file");
                    exit(1);
                }
                if (close(f1) == -1) {
                    perror("close input file");
                    exit(1);
                }
            } else { //Not the first child
                //Set stdin to read from last pipe's output
                if (dup2(pipe_fd[i - 1][0], fileno(stdin))) {
                    perror("dup2 last pipe");
                    exit(1);
                }
            }

            if (i == child_count - 1) {//The last child
                //Set stdout to write to the output file
                int f2 = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                if (dup2(f2, fileno(stdout)) == -1) {
                    perror("dup2 output file");
                    exit(1);
                }
                if (close(f2) == -1) {
                    perror("close output file");
                    exit(1);
                }
            } else { //Not the last child
                //Set stdout to write to this pipe's input
                if (dup2(pipe_fd[i][1], fileno(stdout)) == -1) {
                    perror("dup2 this pipe");
                    exit(1);
                }
            }
            
            //Close all the pipes in pipe_fd
            for (int j = 0; j <= i; j++) {
                if (close(pipe_fd[j][0]) == -1) {
                    perror("close0");
                    exit(1);
                }
                if (close(pipe_fd[j][1]) == -1) {
                    perror("close1");
                    exit(1);
                }
            }
            //Child process will turn into a filter process
            run_command(children[i]);
            //Child process should have terminated
            printf("shouldn't get here");
        }
    }

    //In the parent process, close all the pipes in pipe_fd
    for (i = 0; i < child_count; i++) {
        if (close(pipe_fd[i][0]) == -1) {
            perror("close");
            exit(1);
        }
        if (close(pipe_fd[i][1]) == -1) {
            perror("close");
            exit(1);
        }
    }
    //Wait for all the children to terminate
    int status;
    for (i = 0; i < child_count; i++) {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0){
            printf(ERROR_MESSAGE);
        }
    }
    printf(SUCCESS_MESSAGE);
    return 0;
}
