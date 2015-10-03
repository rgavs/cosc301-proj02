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

char * trim(char * str){
    strtok(str,"/#\n");
    if(str[0] == ' '){
        char * edit = &str[1];
        str = edit;
    }
    if(strrchr(str,' ') != NULL && strlen(strrchr(str,' '))==1){
        char * p = strrchr(str, ' ');
        *p = '\0';
    }               // if there's a space at the end, remove it
    return str;
}

char ** get_cmds(char buff[], int *num_cmds){
    int * num = num_cmds;
    *num = 1;
    do{
        if(!strstr(buff,"&&")){
            break;
        }
        printf("THERES SOME AMPS IN HUR\n");
        (*num)++;
        char * ptr = strstr(buff,"&&") + sizeof(char) * 2; // points to char after substring "&&"
        while(strstr(ptr,"&&")){
            (*num)++;
            ptr = strstr(buff,"&&") + sizeof(char) * 2;
        }
        printf("numbah is -> |%d|\n",*num);
        break;
    }while(true);   // count num_cmds
    char ** cmds = malloc(sizeof(char*) * (*num));
    printf("TESTING | num = %d | strlen buff = %lu\n",*num,strlen(buff));
    if(*num == 1){
        cmds[0] = malloc(sizeof(char) * strlen(buff));
        strncpy(cmds[0],buff,strlen(buff));
        printf("cmds[0] = |%s|\n",cmds[0]);
    }
    else{
        int n = 0;
        char * ptr = buff;
        while(n < *num){
            int x = strstr(ptr,"&&") - ptr;
            cmds[n] = malloc(sizeof(char) * x);
            strncpy(cmds[n],ptr,x);
            cmds[n] = trim(cmds[n]);
            printf("! |%s| !\n",cmds[n]);
            n++;
            ptr = strstr(buff,"&&") + sizeof(char) * 2; // moves pointer to next command
        }
    }
    for(int i = 0; i < *num; i++)
        printf("||%s||\n",cmds[i]);
    return cmds;
}

void free_allocs(char ** cmds, int * num_cmds){
    for(int i = 0; i < *num_cmds;i++)
        free(&cmds[i]);
    return;
}

int main(int argc, char **argv) {
    char buff[1024];
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    int num_cmds = 0;
    char ** cmds;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        trim(buff);
        cmds = get_cmds(buff,&num_cmds);
    }
    free_allocs(cmds,&num_cmds);
    return 0;
}
