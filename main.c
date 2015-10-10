#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>

char** tokenify(const char *s, char * split) {
	char *str_copy = strdup(s);
	//const char * semicolon= ";";
	char *token;
	int count =1;
	for (token=strtok(str_copy, split); token!=NULL; token=strtok(NULL, split)){
		count++;	
	}
	char **tokens = malloc(sizeof(char*)*count);
	char *str_copy2 = strdup(s);
	int index = 0;
	for (token=strtok(str_copy2, split); token!=NULL; token=strtok(NULL, split)){
		tokens[index]=strdup(token);
		index++;
	}
	tokens[index]=NULL;
	free(str_copy);
	free(str_copy2);
	return tokens;
}	

char * get_args(char * cmd, bool * ex, char * modenext){              // takes string and splits into /bin command
    printf("line %d:   In get_args, cmd = |%s|\n",__LINE__,cmd);
    //char ** ret = malloc(sizeof(char *));                         // and flags/options etc
    //ret[0] = malloc(sizeof(char) * 1024);
    //ret[1] = malloc(sizeof(char) * 1024);           // LEAK HERE
    //cmd = trim(cmd);
    if( (strcmp(cmd,"mode sequential")==0) || (strcmp(cmd,"mode  s")==0) ){
        cmd = "s";
        *modenext = 's';
    }
    else if( (strcmp(cmd,"mode parallel")==0) || (strcmp(cmd,"mode p")==0) ){
        cmd = "p";
        *modenext = 'p';
    }
    else if( (strcmp(cmd,"exit")==0) || strstr(cmd,"exit()") ){
            *ex = true;
            cmd = "ex";
    }
   /* if (strstr(cmd," ")){
        strncpy(ret[0], cmd,(strstr(cmd," ") - cmd)/sizeof(char));
        //memcpy(ret[1], &(cmd[strlen(ret[0])]), 1024 - strlen(ret[0]));
    }
    else {
        strcpy(ret[0],cmd);
        //ret[1] = NULL;
    }
     printf("line %d:   In get_args, ret[0] = |%s|\n",__LINE__,ret[0]);
     printf("line %d:   In get_args, ret[1] = |%s|\n",__LINE__,ret[1]);
    return ret; */
    return cmd;
}


int main(int argc, char ** argv) {
    bool ex = false;
    char buff[1024];
    char mode = 's';
    char modenext = 's';
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    char ** cmd_array = NULL;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        if(strchr(buff,'#')){
        char * p = strchr(buff,'#');
        buff[(p - buff)/sizeof(char)] = '\0';
        }
        cmd_array = tokenify(buff, ";");
        int num_process = 0;
        int command =0;
        printf("line %d:   cmd_array | cmd_list->str = |%s|\n",__LINE__,cmd_array[command]);
        while(cmd_array != NULL){
            cmd_array = tokenify(cmd_array[command], "\t\n");
         printf("line %d:   cmd_array | cmd_list->str = |%s|\n",__LINE__,cmd_array[command]);
            cmd_array[command] = get_args(cmd_array[command], &ex, &modenext);
                    printf("line %d:   cmd_array  = |%s|\n",__LINE__,cmd_array[command]);
            if ( (strcmp(cmd_array[command], "s") != 0) || (strcmp(cmd_array[command], "p")!= 0) || (strcmp( cmd_array[command], "ex") != 0 ) ){ // nothing should execute if command is mode 

                if(mode == 's')   
                    int * stat = malloc(sizeof(int));
                    pid_t pid = fork();
                //check return value of fork
                //only wait pid for parent
                //only execcv for child
                    if(pid == 0){

                        if(execv(cmd_array[command], cmd_array) < 0){
                            fprintf(stderr, "line %d:   execv failed: %s\n",__LINE__,strerror(errno));
                        }
                    }
                    else if(pid > 0){
                        waitpid(pid, stat, 0);
                        free(stat);
                    }

                }
                else{                 // this else indicates parallel
                    pid_t pid = fork();
                    if(pid == 0){
                        if(execv(cmd_array[command], cmd_array) < 0){
                            fprintf(stderr, "line %d: execv failed: %s\n",__LINE__,strerror(errno));
                        }
                    num_process += 1;
                    }
                }
            
                if((cmd_array == NULL) && ex){
                    exit(EXIT_SUCCESS);
                }
   
                
            }
        //wait for parallel process
            wait(&num_process);  
            command +=1;
        }
        
        mode = modenext;
    }
    //free(tuple[1]);
    //free(tuple[0]);
    //free(tuple);
    //free_allocs(head);
    free(cmd_array);
    return 0;
}
