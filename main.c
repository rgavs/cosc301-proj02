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
    char * s = str;
    strtok(str,"\n");
    if(str[0] == ' '){
        str = &str[1];
    }
    if(strrchr(str,' ') != NULL && strlen(strrchr(str,' ')) == 1){
        char * p = strrchr(str, ' ');
        *p = '\0';
    }                                                       // if there's a space at the end, remove it
    return s;
}

// Both initializes and appends new strlist to a strlist parameter
// with ->str value as a parameter
strlist * append_list(strlist * curr, char * s){
    strlist * tmp = malloc(sizeof(strlist));
    tmp->str = malloc(sizeof(char) * strlen(s));
    strncpy(tmp->str, s, strlen(s));
    tmp->next = NULL;
    if(curr == NULL){                                         // Initializes
        return tmp;
    }
    else{
        curr->next = tmp;
    }
    printf("%s",tmp->str);
    return curr;
}

strlist * get_cmds(char * buff, int * num_cmds){
    int * num = num_cmds;
    *num = 1;
    if(strchr(buff,'#')){
        char * p = strchr(buff,'#');
        buff[(p - buff)/sizeof(char)] = '\0';
    }
    do{
        if(!strchr(buff,';')){
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
        char * point = buff;
        char * s = malloc(sizeof(char) * 1024);
        int x = 0;
        strlist * item;
        while(strchr(point,';') || strlen(point) > 0){
            printf("line 89:    |%s|\n",point);
            if(strstr(point,";")){
                x = strstr(point,";") - point;             // pointer arithmetic to get size of substring
                printf("line 92:    point is |%s| -- and x is |%d|\n",point,x);
                strncpy(s,point,x);
                s[x] = '\0';
                printf("line 94:    |%s|\n",s);
                trim(s);
            }
            else{
                strncpy(s,point,strlen(point));
                trim(s);
            }
            if(head == NULL){
                head = append_list(head,s);
                item = head;
            }
            else{
                item = append_list(item,s);
                printf("103 %s\n",item->str);
            }
            point = &point[x + 1];
            printf("line 106:    head->str = |%s|\n",head->str);
            if(strcmp(point,"")){
                printf("in if");
                break;
            }
            item->next = malloc(sizeof(strlist));
            item = item->next;
        }
        item->next = NULL;
        printf("line 104:    OUT OF WHILE LOOP\n");
        //free(s);
    }
    printf("line 115:   head->str = |%s| --- head->next->str = |%s|\n",head->str,head->next->str);
    return head;
}

char ** get_args(char * cmd, bool * ex, char * modenext){                              // takes string and splits into /bin command
    printf("in get_args");
    char ** ret = malloc(sizeof(char *) * 2);                         // and flags/options etc
    char * bin = malloc(sizeof(char) * 1024);
    char * argv = malloc(sizeof(char) * 1024);
    argv = "";
    trim(cmd);
    if(strcmp(cmd,"mode sequential")
            || strcmp(cmd,"mode  s")){
        cmd = "s";
        *modenext = 's';
    }
    else if(strcmp(cmd,"mode parallel")
            || strcmp(cmd,"mode p")){
        cmd = "p";
        *modenext = 'p';
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
    while(tmp != NULL){
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
    char modenext = 's';
    char * prompt = "shell# ";
    printf("%s",prompt);
    fflush(stdout);
    int num_cmds = 0;
    strlist * cmd_list;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        char tmp[1024];
        strcpy(tmp,buff);
        strncpy(buff,trim(tmp),1024);
        cmd_list = get_cmds(buff, &num_cmds);
        printf("line 175:   RETURNED FROM get_cmds | cmd_list->str = |%s|\n",cmd_list->str);
        while(cmd_list != NULL){
            printf("in while\n");
            char ** tuple = get_args(cmd_list->str, &ex, &modenext);
            printf("line 166:   mode = %c -- ex = %d-- cmd = |%s| -- argv = |%s|",modenext,ex,tuple[0],tuple[1]);
            if(mode == 's'){
                fork();
                execv(tuple[0],&tuple[1]);
            }/*
            else{                                     // this else indicates parallel
                if(cmd_list->next == NULL){
                    execv(tuple[0],&tuple[1])
                    waitpid();
                }
            }*/
            if(cmd_list->next == NULL && ex){
                exit(EXIT_SUCCESS);
            }
            cmd_list = cmd_list->next;
        }
        mode = modenext;
    }
    free_allocs(cmd_list);
    return 0;
}
