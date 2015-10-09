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
    printf("ln 22:  str = |%s|\n",str);
    char * s = str;
    if(strchr(str,'\n')){
        strtok(str,"\n");
    }
    if(str[0] == ' '){
        str = &str[1];
    }
    if(strrchr(str,' ') != NULL && strlen(strchr(str,' ')) == 1){
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
    strlist * head = NULL;
    if(strchr(buff,'#')){
        char * p = strchr(buff,'#');
        buff[(p - buff)/sizeof(char)] = '\0';
    }
    do{
        if(!strchr(buff,';')){
            break;
        }
        (*num)++;
        char * ptr = strstr(buff,";") + sizeof(char);  // points to char after substring ";"
        while(strchr(ptr,';')){
            (*num)++;
            ptr = strchr(ptr,';') + sizeof(char);
        }
        break;
    }while(true);   // count num_cmds
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
        while(strlen(point) > 0){
            printf("line 84:    |%s|\n",point);
            if(strstr(point,";")){
                x = strstr(point,";") - point;             // pointer arithmetic to get size of substring
                printf("line 87:    point is |%s| -- and x is |%d|\n",point,x);
                strncpy(s,point,x);
                s[x] = '\0';
                printf("line 90:    |%s|\n",s);
                trim(s);
            }
            else{
                strncpy(s,point,strlen(point));
                trim(s);
            }
            if(head == NULL){
                head = append_list(head,s);
                head->next = item;
                printf("line 100:    head->str = |%s|\n",head->str);
            }
            else{
                item = append_list(item,s);
                printf("line 104:   item->str = |%s|\n",item->str);
                item = item->next;
                item = NULL;
            }
            point = &point[x + 1];
            if(strcmp(point,"")){
                printf("in if\n");
                break;
            }
        }
        printf("line 113:    OUT OF WHILE LOOP\n");
        //free(s);
    }
    printf("line 117:   head->str = |%s|\n",head->str);
    return head;
}

char ** get_args(char * cmd, bool * ex, char * modenext){              // takes string and splits into /bin command
    printf("line 121:   In get_args, cmd = |%s|\n",cmd);
    char ** ret = malloc(sizeof(char *) * 2);                         // and flags/options etc
    ret[0] = malloc(sizeof(char) * 1024);
    ret[1] = malloc(sizeof(char) * 1024);
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
        strncpy(ret[0],cmd,(strstr(cmd," ")-cmd)/sizeof(char));
        memcpy(ret[1], &(cmd[strlen(ret[0])]), 1024 - strlen(ret[0]));
    }
    else {
        strcpy(ret[0],cmd);
        ret[1] = "";
    }
    // ret[0] = bin;
    // ret[1] = argv;
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
            printf("line 183:   mode = %c -- ex = %d-- cmd = |%s| -- argv = |%s|\n",modenext,ex,tuple[0],tuple[1]);
        /*  if(mode == 's'){
                fork();
                execv(tuple[0],&tuple[1]);
            }
            else{                                     // this else indicates parallel
                if(cmd_list->next == NULL){
                    execv(tuple[0],&tuple[1])
                    waitpid();
                }
            }*/
            if((cmd_list->next == NULL) && ex){
                exit(EXIT_SUCCESS);
            }
            cmd_list = cmd_list->next;
        }
        mode = modenext;
    }
    free_allocs(cmd_list);
    return 0;
}
