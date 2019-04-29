//Баранов А.В. ПМ2-1 2019
//Terminal, and I/O redirection

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ARG 16     // number of argument
#define ARGLEN 80  // argument length
#define NUMNAMES 2 // number of file names

int address_array(char args[ARG][ARGLEN]);
void exec_comm(char *argv[], char *Addr_filename[]);
void save_adress(char args[ARG][ARGLEN], char *argv[], int num, char *Addr_filename[]);

int main()
{
    char args[ARG][ARGLEN], *argv[ARG], *Addr_filename[NUMNAMES]; // arrays of arguments and addresses
    int num;                                                      // number of argument
    while ((num = address_array(args)) != 0)
    { // check on "ctrl+D"
        if (num == -1)
        {
            continue;
        }
        Addr_filename[0] = NULL;
        Addr_filename[1] = NULL;
        save_adress(args, argv, num, Addr_filename);
        exec_comm(argv, Addr_filename);
    }
    return 0;
}

//Routines

// address_array
int address_array(char args[ARG][ARGLEN])
{
    int x, i = 0, j = 0, in_word = 0, arrow_flag = 0;
    printf(">> ");
    while ((x = getchar()) != EOF)
    {
        if ((x == '>') || (x == '<'))
        { // check on the arrow
            if (j)
            {
                args[i++][j] = '\0';
                j = 0;
            }
            args[i][j++] = x;
            arrow_flag = 1;
            continue;
        }
        if ((x == '\n') && (i == 0) && (in_word == 0))
        { // single enter
            return -1;
        }
        if ((x != ' ') && (x != '\n'))
        { // fillimg in the argument
            args[i][j++] = x;
            in_word = 1;
            arrow_flag = 0;
        }
        if ((x == ' ') && (in_word == 1) && !arrow_flag)
        { // first space after arguments
            args[i++][j] = '\0';
            in_word = 0;
            j = 0;
            continue;
        }
        if (x == '\n')
        { // enter after arguments
            args[i][j] = '\0';
            if (j == 0)
            {
                i--;
            }
            return i + 1;
        }
    }
    return 0;
}

//exec_comm
void exec_comm(char *argv[], char *Addr_filename[])
{
    pid_t child_pid = fork();
    if (!child_pid)
    {
        if (Addr_filename[1])
        {
            int f1 = open(Addr_filename[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (f1 == -1)
            {
                perror("open");
                exit(1);
            }
            if (-1 == dup2(f1, STDOUT_FILENO))
            {
                perror("dup2");
                exit(1);
            }
        }
        if (Addr_filename[0])
        {

            int f0 = open(Addr_filename[0], O_RDONLY, 0666);
            if (f0 == -1)
            {
                perror("open");
                exit(1);
            }
            if (-1 == dup2(f0, STDIN_FILENO))
            {
                perror("dup2");
                exit(1);
            }
        }
        if (execvp(argv[0], argv) == -1)
        {

            perror("execvp");
            exit(1);
        }
        exit(0);
    }
    pid_t pid = wait(NULL);
    if (pid == -1)

    {
        perror("wait");
        exit(1);
    }
}
// save_adress
void save_adress(char args[ARG][ARGLEN], char *argv[], int num, char *Addr_filename[])
{
    int n = 0;
    for (int i = 0; i < num; i++)
    {
        if (args[i][0] == '>')
        {
            Addr_filename[1] = &args[i][1];
            continue;
        }
        if (args[i][0] == '<')
        {
            Addr_filename[0] = &args[i][1];
            continue;
        }
        argv[n++] = args[i];
    }
    argv[n] = NULL;
}
