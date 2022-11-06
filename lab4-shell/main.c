#include <stdio.h>
#include <assert.h>
#include <unistd.h>
extern char **environ;

extern char **environ;
#include <stdlib.h> 

static void sigintHandler(int sig)
{
    printf("pid: %d", getpid());
    printf("Caught singal %d\n", sig);
}




int main(int argc, char** argv) {

    char cwd[1024];
    
    /* get workng directory */
    // if(getcwd(cwd,sizeof(cwd)) == NULL) {
    //         perror("getcwd() error");
    // } else {
    //         printf("current working directory is: %s\n", cwd);
    // }

    /* change workng directory */
    // if(chdir("/")!=0)
    //     perror("chdir() error");

    // if(getcwd(cwd,sizeof(cwd)) == NULL) {
    //     perror("getcwd() error");
    // } else {
    //     printf("current working directory is: %s\n", cwd);
    // }
    

    /* print environment variables */
    // for (char **env = environ; *env; ++env)
    //     printf("%s\n", *env);

    /* set environmnet variable */
    if (setenv("PATH","PATH=/:/home/userid",1) == -1)
        perror("putenv() error");
    else 
        printf("%s\n", getenv("PATH"));
    printf("%s\n", getenv("PATH"));

    /* wait for child */
    pid_t pid;
    // pid = fork();
    // if ( pid == 0) {
    //     printf("Child is runnig\n");
    //     char *args[] = { "LS", NULL};
    //     execv("/bin/ls", args);
    // } else {
    //     printf("Suspending parent\n");
    //     int status = 0;
    //     waitpid(pid, &status,WUNTRACED );
    //     printf("Resumeing parent\n");
    // }

    /* backgroud for child */
    // pid = fork();
    // if ( pid == 0) {
    //     if (signal(SIGINT, sigintHandler) == SIG_ERR) {
    //         perror("signal SIGINT");
    //         exit(EXIT_FAILURE);
    //     } 
    //     printf("Child(%d) start\n", getpid());
    //     sleep(10);
    //     // char *args[] = { "LS", NULL};
    //     // execv("/bin/ls", args);
    //     printf("Child(%d) end\n", getpid());
    // } else {
    //     printf("Caller(%d):[1] %d\n", pid);
    //     if (signal(SIGINT, sigintHandler) == SIG_ERR) {
    //         perror("signal SIGINT");
    //         exit(EXIT_FAILURE);
    //     } 
    //     pid_t w;
    //     int status = 0;
    //     do {
    //         w = waitpid(pid, &status,WUNTRACED | WCONTINUED );
    //         if (WIFEXITED(status)) {
    //                 printf("exited, status=%d\n", WEXITSTATUS(status));
    //             } else if (WIFSIGNALED(status)) {
    //                 printf("killed by signal %d\n", WTERMSIG(status));
    //             } else if (WIFSTOPPED(status)) {
    //                 printf("stopped by signal %d\n", WSTOPSIG(status));
    //             } else if (WIFCONTINUED(status)) {
    //                 printf("continued\n");
    //             }
    //     } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    //     printf("[1] + done sleep 10\n");
    //     exit(EXIT_SUCCESS);
    // }

    char *s = "path=jzm";

                            printf("D0\n");
                            printf("s %s\n", s);
    char buffer_var[1024] = {0}, buffer_val[1024] = {0};
    int offset = 0;
    for (; s != NULL; s++ ) {
        if (*s != '=') {
                            printf("D3\n");
                            printf("%c\n", *s);
            *(buffer_var+offset) = *s;
            offset++;
        } else 
            break;
    }
                            printf("D1\n");
                            printf("buffer_var %s\n", buffer_var);

    offset=0;
    s++; // move '='
    for (; *s != '\0'; s++ ) {
        *(buffer_val + offset) = *s;
        offset++;
    }

                            printf("D2\n");
                            printf("buffer_val %s\n", buffer_val);

    printf("\n");
    return 0;
}
