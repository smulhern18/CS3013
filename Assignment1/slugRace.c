//
// Created by sjmulhern on 2/7/2021.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char** argv) {

    pid_t childProcess;
    pid_t childProcesses[4] = {0, 0, 0, 0};
    int statuses[4] = {1, 1, 1, 1};
    int i = 0;
    for (i = 0; i < 4; i++) {
        childProcess = fork();

        if (childProcess == 0) {
            char** buffer = (char**) calloc(sizeof(calloc(sizeof(char), 16)), 3);
            buffer[0] = "./slug";
            switch (i) {
                case 0:
                    buffer[1] = "1";
                    break;
                case 1:
                    buffer[1] = "2";
                    break;
                case 2:
                    buffer[1] = "3";
                    break;
                case 3:
                    buffer[1] = "4";
                    break;
            }
            buffer[2] = NULL;
            execv("./slug", buffer);
        } else {
            childProcesses[i] = childProcess;
        }
    }
    while (statuses[0] || statuses[1] || statuses[2] || statuses[3]) {
        for (i = 0; i < 4; i++) {
            int status = -1;
            waitpid(childProcesses[i], &status, WNOHANG);
            if (WIFEXITED(status)) {
                printf("Child %d has crossed the finishline!\n", (int)childProcesses[i]);
                statuses[i] = 0;
            }
        }

        printf("The Current Slugs racing are: ");
        for (i = 0; i < 4; i++) {
            if (statuses[i] != 0) {
                printf("%d ", (int)childProcesses[i]);
            }
        }
        printf("\n");
        usleep(250000);
    }
    printf("All Processes have finished!\n");
    return 0;
}