
#include <stdio.h>
#include <stdlib.h> /* getenv */
#include <string.h> /* strdup */
#include <sys/stat.h> /* stat */
#include <unistd.h> /* fork */

#include "posixfunc.h"

extern char **environ;

char *get_env_var(char *name){
    char *a = getenv((char *)(name+1));
    return a;
}

int file_exists(const char *filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0 ? 1 : 0;
}


char *find_cmd(char* cmd) {

    char *a = getenv("PATH");
    char buffer[1024] = {0};
    int offset = 0;
    for ( ;; a++) {
                            // printf("D0\n");
                            // printf("%p %c\n", a, *a);
        if (  *a != ':' &&   *a != '\0') {
            *(buffer+offset) = *a;
            offset++;

        } else {
            *(buffer+offset) = '/'; 
            offset++;
                            // printf("D1\n");
                            // printf("%s\n", buffer);
            for (char *c = cmd; *c != '\0'; c++ ) {
                *(buffer+offset) = *c;
                offset++;
            }
                            // printf("D2\n");
                            // printf("cmd %s  abs %s\n",cmd, buffer);
            if ( file_exists(buffer) == 1) {
                            // printf("D3\n");
                            // printf("abs %s\n", buffer);
                char *a = strdup(buffer);
                return a;
            } else {
                memset(buffer,0, 1014);
                offset = 0;
            }
            if ( *a == '\0') break;
        }
    }
    return NULL;
}
void cmd_pwd() {
    char cwd[1024];
    /* get workng directory */
    if(getcwd(cwd,sizeof(cwd)) == NULL) {
        perror("getcwd() error");
    } else {
        printf("%s\n", cwd);
    }
} 
    
  /* change workng directory */
void cmd_cd(char* path) {
    if (chdir(path)!=0)
        perror("chdir() error");
}

void cmd_echo(char** p) {
    for (; *p != NULL; p++ ) {
        printf("%s ", *p);
    }
    printf("\n");
}

void cmd_set(char* s) {
                            // printf("D0\n");
                            // printf("s %s\n", s);
    char buffer_var[1024] = {0}, buffer_val[1024] = {0};
    int offset = 0;
    for (; *s != '0'; s++ ) {
        if (*s != '=') {
                            // printf("D3\n");
                            // printf("%c\n", *s);
            *(buffer_var+offset) = *s;
            offset++;
        } else 
            break;
    }
                            // printf("D1\n");
                            // printf("buffer_var %s\n", buffer_var);

    offset=0;
    s++; // move '='
    for (; *s != '\0'; s++ ) {
        *(buffer_val + offset) = *s;
        offset++;
    }
                            // printf("D2\n");
                            // printf("buffer_val %s\n", buffer_val);
   if (setenv(buffer_var,buffer_val,1) == -1)
        perror("putenv() error");
}




static void sigintHandler(int sig)
{
    printf("pid: %d", getpid());
    printf("Caught singal %d\n", sig);
}

int exe_cmd(char *cmd, int argc, char** argv, int bg){
    /* wait for child */
    // printf("exe_cmd %s %s %s", cmd, *argv, *(argv+1));

    pid_t pid;
    pid = fork();
    if ( bg == 0 ) {
        if ( pid == 0) {
            execv(cmd, argv);
        } else {
            int status = 0;
            waitpid(pid, &status,WUNTRACED );
        };
    } else {
        if ( pid == 0) {
            if (signal(SIGINT, sigintHandler) == SIG_ERR) {
                perror("signal SIGINT");
                exit(EXIT_FAILURE);
            } 
            execv(cmd, argv);
            printf("Child(%d) end\n", getpid());
        } else {
            printf("[1] %d\n", pid);
            if (signal(SIGINT, sigintHandler) == SIG_ERR) {
                perror("signal SIGINT");
                exit(EXIT_FAILURE);
            } 
            pid_t w;
            int status = 0;
            do {
                w = waitpid(pid, &status,WUNTRACED | WCONTINUED );
                if (WIFEXITED(status)) {
                        printf("exited, status=%d\n", WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("killed by signal %d\n", WTERMSIG(status));
                    } else if (WIFSTOPPED(status)) {
                        printf("stopped by signal %d\n", WSTOPSIG(status));
                    } else if (WIFCONTINUED(status)) {
                        printf("continued\n");
                    }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            printf("[1] + done %s\n", *argv);
            // exit(EXIT_SUCCESS);
        }
    }
    return 0;
}

