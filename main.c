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

struct list{
    char * str[1024];
}; typedef struct list _list;

char ** get_cmds(char buff[], int *num_cmds){
    int * num = num_cmds;
    strtok(buff,"\n");
    printf("|%s|\n",buff);
    for(int i = 0; i < strlen(buff) - 1; i++){
        if(buff[i] == '#'){
            buff[i] = '\0';
            break;
        }
        if(buff[i] == '&' && buff[i + 1] == '&')
            num_cmds ++;
    }
    char ** cmds[(*num_cmds)] = malloc(sizeof(char)*(*num_cmds));
    if(*num == 1){
        strcpy(*cmds[0],buff);
    }
    else{
        int n = 0;
        int tmp = 0;
        while(n < *num){
            for(int j = 0; j < strlen(buff) - 1; j++){
                if(buff[j] == '&' && buff[j + 1] == '&'){
                    strncpy(*cmds[n],&buff[tmp],j - tmp); // copies from previous word (or start) until j
                    tmp = j;
                    strncpy(*cmds[j - n],"\0",1);
                }
            }
        }
    }
    for(int i = 0; i < *num; i++)
        printf("%s\n",*cmds[i]);
    return *cmds;
}

int main(int argc, char **argv) {
    char buff[1024];
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    int num_cmds;
    char ** cmds;
    while(fgets(buff,1024,stdin) != NULL){
        cmds = get_cmds(buff,&num_cmds);

    }
    return 0;
}
