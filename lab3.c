//Баранов А.В. ПМ2-1 2019
//Terminal, and I/O redirection, and conveyor

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
void exec_comm(char *argv[2][ARG], char *Addr_filename[], int fl_p);
int save_adress(char args[ARG][ARGLEN], char *argv[2][ARG], int num, char *Addr_filename[]);
void pipe_in(int fd[]);
void pipe_out(int fd[]);
int main()
{
    char args[ARG][ARGLEN], *argv[2][ARG], *Addr_filename[NUMNAMES]; // arrays of arguments and addresses
    int num, fl_p = 0;                                          // number of argument
    while ((num = address_array(args)) != 0)// check on "ctrl+D"
    {
        if (num == -1)
        {
            continue;
        }
        Addr_filename[0] = NULL;
        Addr_filename[1] = NULL;
        fl_p = save_adress(args, argv, num, Addr_filename);
        exec_comm(argv, Addr_filename, fl_p);
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
        if ((x == '>') || (x == '<') || (x == '|'))
        { // check on the arrow and pipe
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
void exec_comm(char *argv[2][ARG], char *Addr_filename[], int fl_p)
{
    int fd[2];
    if (-1 == pipe(fd))
    {
        perror("pipe");
        return;
    }
    pid_t child_pid1 = fork();
    if (!child_pid1)
    {
        if (fl_p)
        {
            pipe_out(fd);
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
        if (Addr_filename[1] && !fl_p)
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
        if (execvp(argv[0][0], argv[0]) == -1)
        {
            perror("execvp");
            exit(1);
        }
        exit(0);
    }
    if (fl_p)
    {
        pid_t child_pid2 = fork();
        if (!child_pid2)
        {
            pipe_in(fd);
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
            if (execvp(argv[1][0], argv[1]) == -1)
            {
                perror("execvp");
                exit(1);
            }
            exit(0);
        }
        close(fd[0]);
        close(fd[1]);
        pid_t pid2 = wait(NULL);
        if (pid2 == -1)
        {
            perror("wait");
            exit(1);
        }
    }
    pid_t pid = wait(NULL);
    if (pid == -1)
    {
        perror("wait");
        exit(1);
    }
    if (!fl_p)
    {
        close(fd[0]);
        close(fd[1]);
    }
}
// save_adress
int save_adress(char args[ARG][ARGLEN], char *argv[2][ARG], int num, char *Addr_filename[])
{
    int n = 0, j = 0;
    for (int i = 0; i < num; i++)
    {
        if (args[i][0] == '|')
        {
            argv[j++][n] = NULL;
            n = 0;
            argv[j][n++] = &args[i][1];
            continue;
        }
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
        argv[j][n++] = args[i];
    }
    argv[j][n] = NULL;
    return j;
}
//pipe_in
void pipe_in(int fd[])
{
    close(fd[1]);
    if (-1 == dup2(fd[0], STDIN_FILENO))
    {
        perror("dup2");
        exit(1);
    }
}
//pipe_out
void pipe_out(int fd[])
{
    close(fd[0]);
    if (-1 == dup2(fd[1], STDOUT_FILENO))
    {
        perror("dup2");
        exit(1);
    }
}