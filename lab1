//Баранов А.В. ПМ2-1 2019
//Terminal

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define ARG 16     // number of argument
#define ARGLEN 80  // argument length

int address_array(char args[ARG][ARGLEN]);
void exec_comm(char *argv[]);

int main() {
    char args[ARG][ARGLEN], *argv[ARG]; // arrays of arguments and addresses
    int num;  // number of argument
    while ((num = address_array(args)) != 0) { // check on "ctrl+D"
          if (num == -1) {
             continue;
          }
          for (int i = 0; i < num; i++) {
              argv[i] = args[i];// filling in the address array
          }
          argv[num] = NULL;
          exec_comm(argv);
    }
    return 0;
}

//Routines

// address_array
int address_array(char args[ARG][ARGLEN]){
    int x, i = 0, j = 0, in_word = 0;
    printf(">> ");
    while ((x = getchar()) != EOF) {
          if ((x == '\n') && (i == 0) && (in_word == 0)) { // single enter
             return -1;
          }
          if ((x != ' ') && (x != '\n')) {                // fillimg in the argument
             args[i][j++] = x;
             in_word = 1;
          }          
          if ((x == ' ') && (in_word == 1)) {             // first space after arguments
             args[i++][j] = '\0';
             in_word = 0;
             j = 0;
             continue;
          }
          if (x == '\n') {                               // enter after arguments
             args[i][j] = '\0';
             return i+1;
          }
    }
    return 0;
}

//exec_comm
void exec_comm(char *argv[]) {
     pid_t child_pid = fork();
     if (!child_pid) {
        if (execvp(argv[0], argv) == -1) {
           perror("execvp");
        }
        exit(0);
     }
     pid_t pid = wait(NULL);
     if (pid == -1) {
        perror("wait");
        exit(0);
     }
}
