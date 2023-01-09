
#include <stdio.h>
#include <stdlib.h> /* malloc getenv */
#include <string.h> /* strdup */
#include <sys/stat.h> /* stat */
#include <unistd.h> /* fork */
#include <pthread.h>
#include <signal.h>


                    
                    
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



int fg = 0;
static void sigintHandler(int sig)
{
    // printf("pid: %d", getpid());
    // printf("Caught singal %d\n", sig);
    if ( fg == 1) {
        // ddo nothing
    } else {
        abort();
    }
}


struct process {
    int pid;
    char* rel_cmd;
    int jobid;
    int status;
    struct process* next;
};

pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER; 
struct process *process_list = NULL;

struct process *new_process(struct process* next, int pid, char* rel_cmd, int jobid) { 
    struct process *p = malloc(sizeof(struct process));
    if (!p) {
        // error("out of space");
        // exit(0);
    }
    memset(p, 0, sizeof(struct process));

    p->pid = pid;
    p->rel_cmd = rel_cmd;
    p->jobid = jobid;
    p->status = 0;
    p->next = next;
    return p;
}


void *killer_thread(void *args) {
    int pid = (int)args;
    sleep(10);
    kill(pid, SIGINT);
    pthread_exit(NULL);
}

void *handle_process_signals(void *args) {
    while (1) {
        struct process *list = process_list;
        while (list != NULL && list->status == 0) { // cycle through process list
            pid_t w;
            int status = 0;
            // do {
                w = waitpid(list->pid, &status, WUNTRACED | WCONTINUED );
                if (WIFEXITED(status)) {
                        printf("[%d] + done %s\n", list->jobid, list->rel_cmd);
                        printf("exited, status=%d\n", WEXITSTATUS(status));
                        pthread_mutex_lock(&process_mutex); 
                        list->status = 1;
                        pthread_mutex_unlock(&process_mutex); 
                        printf("> "); // HACK SHOULD be callback to main.c
                        fflush(stdout);
                    } else if (WIFSIGNALED(status)) {
                        printf("killed by signal %d\n", WTERMSIG(status));
                    } else if (WIFSTOPPED(status)) {
                        printf("stopped by signal %d\n", WSTOPSIG(status));
                    } else if (WIFCONTINUED(status)) {
                        printf("continued\n");
                    }
            // } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        sleep(1);
    }
    pthread_exit(NULL);
}
    
int exe_cmd(char *cmd, char** argv, int bg) {
    /* wait for child */
    // printf("exe_cmd: %s %s %s\n", cmd, *argv, *(argv+1));

    pid_t pid;
    pid = fork();
    pthread_t threadid;

    if ( bg == 0 ) {
        if ( pid == 0) {
            execv(cmd, argv);
        } else {
            if (signal(SIGINT, sigintHandler) == SIG_ERR) {
                perror("signal SIGINT ???");
                exit(EXIT_FAILURE);
            } 
            int status = 0;
            fg = 1;
            pthread_create(&threadid, NULL, killer_thread, (void *)pid);
            pthread_detach(threadid); 
            waitpid(pid, &status,WUNTRACED );
            fg = 0;
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
            struct process *p;
            if (process_list == NULL)  {
                /* start thread to listing to events */
                pthread_create(&threadid, NULL, handle_process_signals, NULL);
                pthread_detach(threadid);
                pthread_mutex_lock(&process_mutex); 
                process_list = new_process(NULL, pid, strdup(*argv), 1);
                pthread_mutex_unlock(&process_mutex);
            } else {
                process_list = new_process(process_list, pid, strdup(*argv), process_list->jobid++);
            }
            
            printf("[%d] %d\n", process_list->jobid, pid);

            if (signal(SIGINT, sigintHandler) == SIG_ERR) {
                perror("signal SIGINT");
                exit(EXIT_FAILURE);
            }
        } 
    }
    return 0;
}

