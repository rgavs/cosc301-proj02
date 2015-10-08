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

// Linked list struct
struct str_list{
    char * str;
    struct str_list * next;
}; typedef struct str_list strlist;

char * trim(char * str){
    strtok(str,"\n");
    if(str[0] == ' '){
        char * edit = &str[1];
        str = edit;
    }
    if(strrchr(str,' ') != NULL && strlen(strrchr(str,' ')) == 1){
        char * p = strrchr(str, ' ');
        *p = '\0';
    }                                                       // if there's a space at the end, remove it
    return str;
}

// Both initializes and appends new strlist to a strlist parameter
// with ->str value as a parameter
strlist * append_list(strlist * curr, char * s){
    strlist * tmp;
    char * st = trim(s);
    if(curr==NULL){                                         // Initializes
        tmp = malloc(sizeof(strlist));
        tmp->str = malloc(strlen(st));
        strncpy(tmp->str,st,strlen(st));
        tmp->next = NULL;
    }
    else{
        curr->next = malloc(sizeof(strlist));
        tmp = curr->next;
        tmp->str = malloc(strlen(st));
        strncpy(tmp->str,st,strlen(st));
        tmp->next = NULL;
    }
    return tmp;
}

strlist * get_cmds(char buff[], int * num_cmds){
    int * num = num_cmds;
    *num = 1;
    if(strchr(buff,'#')){
        char * p = strchr(buff,'#');
        buff[(p - buff)/sizeof(char)] = '\0';
    }
    do{
        if(!strstr(buff,";")){
            break;
        }
        printf("line 61:    MULTI-COMMAND STRING\n");
        (*num)++;
        char * ptr = strstr(buff,";") + sizeof(char);  // points to char after substring ";"
        while(strchr(ptr,';')){
            (*num)++;
            ptr = strchr(ptr,';') + sizeof(char);
        }
        break;
    }while(true);   // count num_cmds
    printf("line 71:    TESTING | *num = %d | strlen buff = %lu\n",*num,strlen(buff));
    strlist * head = NULL;
    if(*num == 1){
        head = append_list(NULL,buff);
        printf("line 75:    head->str = |%s|\n",head->str);
        return head;
    }
    else{
        // int n = 0;
        strlist * item = head;
        char * point = buff;
        char * s = malloc(sizeof(char)*1024);
        int x = 0;
        while(strstr(point,";") || strlen(point) > 0){
            if(strstr(point,";")){
                x = strstr(point,";") - point;             // pointer arithmetic to get size of substring
                strncpy(s,point,x);
            }
            else{
                strncpy(s,point,strlen(point));
            }
            item = append_list(item,s);
            point = &point[x+2];
            printf("line 95:    point = |%s|\n",point);
        }
        printf("line 97:    OUT OF WHILE LOOP\n");
        free(s);
    }
    return head;
}

char ** get_args(char * cmd, bool * ex, char * mode){                              // takes string and splits into /bin command
    char ** ret = malloc(sizeof(char *) * 2);                         // and flags/options etc
    char * bin = malloc(sizeof(char) * 1024);
    char * argv = malloc(sizeof(char) * 1024);
    argv = "";
    trim(cmd);
    if(strcmp(cmd,"mode sequential")
            || strcmp(cmd,"mode  s")){
        cmd = "s";
        *mode = 's';
    }
    else if(strcmp(cmd,"mode parallel")
            || strcmp(cmd,"mode p")){
        cmd = "p";
        *mode = 'p';
    }
    else if(strcmp(cmd,"exit") || strstr(cmd,"exit()")){
            *ex = true;
            cmd = "ex";
    }
    if (strstr(cmd," ")){
        strncpy(bin,cmd,(strstr(cmd," ")-cmd)/sizeof(char));
        memcpy(argv, &(cmd[strlen(bin)]), 1024 - strlen(bin));
    }
    else {
        strcpy(bin,cmd);
    }
    ret[0] = bin;
    ret[1] = argv;
    return ret;
}

void free_allocs(strlist * head){
    strlist * tmp = head;
    strlist * tmp2 = tmp->next;
    while(tmp!=NULL){
        free(tmp);
        tmp = tmp2;
        tmp2 = tmp->next;
    }
    return;
}

int main(int argc, char ** argv) {
    bool ex = false;
    char buff[1024];
    char mode = 's';
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    int num_cmds = 0;
    strlist * cmd_list;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        trim(buff);
        cmd_list = get_cmds(buff,&num_cmds);
        printf("line 124:   RETURNED FROM get_cmds\n");
        while(cmd_list != NULL){
            char ** tuple = get_args(cmd_list->str, &ex, &mode);
            printf("mode = %c -- ex = %d-- cmd = |%s| -- argv = |%s|",mode,ex,tuple[0],tuple[1]);
            if(mode == 's'){
                fork();
                execv(tuple[0],&tuple[1]);
            }/*
            else{                                     // this else indicates parallel
                if(cmd_list->next == NULL){
                    execv(tuple[0],&tuple[1])
                }
            }*/
            if(cmd_list->next == NULL && ex){
                exit(EXIT_SUCCESS);
            }
            cmd_list = cmd_list->next;
        }
    }
    free_allocs(cmd_list);
    return 0;
}
