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
    strtok(str,"/#\n");
    if(str[0] == ' '){
        char * edit = &str[1];
        str = edit;
    }
    if(strrchr(str,' ') != NULL && strlen(strrchr(str,' '))==1){
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
    do{
        if(!strstr(buff,"&&")){
            break;
        }
        printf("line 61:    MULTI-COMMAND STRING\n");
        (*num)++;
        char * ptr = strstr(buff,"&&") + sizeof(char) * 2;  // points to char after substring "&&"
        while(strstr(ptr,"&&")){
            printf("line 66:    TESTING -- ptr = %s -- *num = %d\n",ptr,*num);
            (*num)++;
            ptr = strstr(ptr,"&&") + sizeof(char) * 2;
        }
        printf("line 69:    numbah is -> |%d|\n",*num);
        break;
    }while(true);   // count num_cmds
    printf("line 72:    TESTING | num = %d | strlen buff = %lu\n",*num,strlen(buff));
    strlist * head = NULL;
    if(*num == 1){
        head = append_list(NULL,buff);
        printf("line 77:    head->str = |%s|\n",head->str);
        return head;
    }
    else{
        // int n = 0;
        strlist * item = head;
        char * point = buff;
        char * s = malloc(sizeof(char)*1024);
        int x = 0;
        while(strstr(point,"&&") || strlen(point) > 0){
            if(strstr(point,"&&")){
                x = strstr(point,"&&") - point;             // pointer arithmetic to get size of substring
                strncpy(s,point,x);
            }
            else{
                strncpy(s,point,strlen(point));
            }
            item = append_list(item,s);
            printf("line 95:    item->str = |%s| -- x + 2 = %d -- point = |%s| -- strlen(point) = %lu\n",item->str,(x+2),point,strlen(point)); // FIX THIS LINE!!
            point = &point[x+2];
            printf("line 97:    point = |%s|\n",point);
        }
        printf("line 99:    OUT OF WHILE LOOP\n");
        free(s);
    }
    return head;
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
    char buff[1024];
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    int num_cmds = 0;
    strlist * cmd_list;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        trim(buff);
        cmd_list= get_cmds(buff,&num_cmds);
        printf("line 131:   RETURNED FROM get_cmds");
    }
    free_allocs(cmd_list);
    return 0;
}
