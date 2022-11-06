
 
 /* get enviroment variable */
char *get_env_var(char *name);
char *find_cmd(char* cmd);
void cmd_pwd();
void cmd_cd(char* path);
void cmd_echo(char** p);
int exe_cmd(char *cmd, int argc, char** argv, int bg);
void cmd_set(char* s);

