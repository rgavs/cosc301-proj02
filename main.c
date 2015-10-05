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

// Both initializes and appends new strlist to a strlist parameter
// with ->str value as a parameter
strlist * append_list(strlist * curr,char * s){
    strlist * tmp;
    if(curr==NULL){                                         // Initializes
        tmp = malloc(sizeof(strlist));
        tmp->str = malloc(strlen(s));
        strncpy(tmp->str,s,strlen(s));
        tmp->next = NULL;
    }
    else{
        curr->next = malloc(sizeof(strlist));
        tmp = curr->next;
        tmp->str = malloc(strlen(s));
        strncpy(tmp->str,s,strlen(s));
        tmp->next = NULL;
    }
    return tmp;
}

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

strlist * get_cmds(char buff[], int *num_cmds){
    int * num = num_cmds;
    *num = 1;
    do{
        if(!strstr(buff,"&&")){
            break;
        }
        printf("THERES SOME AMPS IN HUR\n");
        (*num)++;
        char * ptr = strstr(buff,"&&") + sizeof(char) * 2;  // points to char after substring "&&"
        while(strstr(ptr,"&&")){
            (*num)++;
            ptr = strstr(buff,"&&") + sizeof(char) * 2;
        }
        printf("numbah is -> |%d|\n",*num);
        break;
    }while(true);   // count num_cmds
    printf("TESTING | num = %d | strlen buff = %lu\n",*num,strlen(buff));

    strlist * head = append_list(NULL,buff);
    printf("head->str = |%s|\n",head->str);

    if(*num == 1){
        return head;
    }
    else{
        // int n = 0;
        strlist * item = head;
        char * point = buff;
        char * s = malloc(sizeof(char)*1024);
        for(int i = 0; i < *num; i++){
            int x = strstr(point,"&&") - point;             // pointer arithmetic to get size of substring
            strncpy(s,point,x);
            item = append_list(item,s);
        }
        free(s);
        // while(n < *num){
        //     int x = strstr(ptr,"&&") - ptr;
        //     cmds[n] = malloc(sizeof(char) * x);
        //     strncpy(cmds[n],ptr,x);
        //     cmds[n] = trim(cmds[n]);
        //     printf("! |%s| !\n",cmds[n]);
        //     n++;
        //     ptr = strstr(buff,"&&") + sizeof(char) * 2;     // moves pointer to next command
        // }
    }
    // for(int j = 0; j < *num; j++)
    //     printf("||%s||\n",cmds[j]);
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

int main(int argc, char **argv) {
    char buff[1024];
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    int num_cmds = 0;
    strlist * cmd_list;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        trim(buff);
        cmd_list= get_cmds(buff,&num_cmds);
    }
    free_allocs(cmd_list);
    return 0;
}
