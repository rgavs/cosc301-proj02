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

// Linked list struct
struct str_list{
    char * str;
    struct str_list * next;
}; typedef struct str_list strlist;

char * trim(char * str){
    printf("ln %d:  str = |%s|\n",__LINE__,str);
    char * s = str;
    /*if(strchr(s,'\n')){
        strtok(s,"\n");
    }*/
    //s[strlen(s) - 1] = '\0';
    while(str[0] == ' '){
        s = &s[1];
    }
    while(isspace(s[strlen(s)-1])){
        s[strlen(s) - 1] = '\0';
    }                                                      // if there's a space at the end, remove it
    return s;
}

// Both initializes and appends new strlist to a strlist parameter
// with ->str value as a parameter
strlist * append_list(strlist * curr, char * s){
    strlist * add = malloc(sizeof(strlist));
    add->str = strdup(trim(s));
    //malloc(sizeof(char) * strlen(s));
    //strncpy(add->str, trim(s), strlen(trim(s)));
    add->next = NULL;
    if(curr == NULL){                                         // Initializes
        return add;
    }
    else{
        curr->next = add;
    }
    printf("%s",add->str);
    return add;
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
        char * ptr = strstr(buff,";") + sizeof(char);       // points to char after substring ";"
        while(strchr(ptr,';')){
            (*num)++;
            ptr = strchr(ptr,';') + sizeof(char);
        }
        break;
    }while(true);   // count num_cmds
    if(*num == 1){
        head = append_list(NULL,buff);
        printf("line %d:    head->str = |%s|\n",__LINE__,head->str);
        return head;
    }
    else{
        char * point = buff;
        char * s = malloc(sizeof(char) * 1024);
        int x = 0;
        while(strlen(point) > 0){
            strlist * item = head;                          // temporary strlist
            printf("line %d:    |%s|\n",__LINE__,point);
            if(strchr(point,';')){
                x = strchr(point,';') - point;              // pointer arithmetic to get size of substring
                printf("line %d:    point is |%s| -- and x is |%d|\n",__LINE__,point,x);
                strncpy(s,point,x);
                s[x] = '\0';
                printf("line %d:    |%s|\n",__LINE__,s);
                trim(s);
            }
            else{
                strncpy(s,point,strlen(point));
                s = trim(s);
            }
            if(head == NULL){
                head = append_list(head,s);
                head->next = item;
                printf("line %d:    head->str = |%s|\n",__LINE__,head->str);
            }
            else{
                item = append_list(item,s);
                printf("line %d:   item->str = |%s|\n",__LINE__,item->str);
            }
            point = &point[x + 1];
            if(strcmp(point,"")){
                break;
            }
        }
        printf("line %d:    OUT OF WHILE LOOP\n",__LINE__);
        free(s);
    }
    printf("line %d:   head->str = |%s|\n",__LINE__,head->str);
    return head;
}

char ** get_args(char * cmd, bool * ex, char * modenext){              // takes string and splits into /bin command
    printf("line %d:   In get_args, cmd = |%s|\n",__LINE__,cmd);
    char ** ret = malloc(sizeof(char *) * 2);                         // and flags/options etc
    ret[0] = malloc(sizeof(char) * 1024);
    ret[1] = malloc(sizeof(char) * 1024);           // LEAK HERE
    trim(cmd);
    if(strcmp(cmd,"mode sequential") || strcmp(cmd,"mode  s")){
        cmd = "s";
        *modenext = 's';
    }
    else if(strcmp(cmd,"mode parallel") || strcmp(cmd,"mode p")){
        cmd = "p";
        *modenext = 'p';
    }
    else if(strcmp(cmd,"exit") || strstr(cmd,"exit()")){
            *ex = true;
            cmd = "ex";
    }
    if (strstr(cmd," ")){
        strncpy(ret[0], cmd,(strstr(cmd," ") - cmd)/sizeof(char));
        memcpy(ret[1], &(cmd[strlen(ret[0])]), 1024 - strlen(ret[0]));
    }
    else {
        strcpy(ret[0],cmd);
        ret[1] = NULL;
    }
    return ret;
}

void free_allocs(strlist * head){
    strlist * tmp = head;
    strlist * tmp2 = tmp->next;
    while(tmp != NULL){
        tmp2 = tmp;
        tmp = tmp2->next;
        free(tmp2->str);
        free(tmp2);
    }
    printf("line %d: Allocs freed",__LINE__);
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
    strlist * head;
    strlist * cmd_list;
    char ** tuple;
    while(fgets(buff,1024,stdin) != NULL && !strstr(buff,"^D")){
        char tmp[1024];
        strcpy(tmp,buff);
        strncpy(buff,trim(tmp),1024);
        head = get_cmds(buff, &num_cmds);
        cmd_list = head;
        printf("line %d:   RETURNED FROM get_cmds | cmd_list->str = |%s|\n",__LINE__,cmd_list->str);
        int num_process = 1;
        while(cmd_list != NULL){
            tuple = get_args(cmd_list->str, &ex, &modenext);
            printf("line %d:   mode = %c -- ex = %d -- cmd = |%s| -- argv = |%s|\n",__LINE__,modenext,ex,tuple[0],tuple[1]);
            if(mode == 's' && strncmp(tuple[0],"s",strlen(tuple[0])) != 0){   // nothing should execute if command is mode
                int * stat = malloc(sizeof(int));
                pid_t pid = fork();
                //check return value of fork
                //only wait pid for parent
                //only execcv for child
                if(pid == 0){
                    printf("line %d:    tuple[0] = |%s| and tuple[1] = |%s|\n",__LINE__,tuple[0],tuple[1]);
                    if(execv(tuple[0], &tuple[1]) < 0){
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
                num_process += 1;
                if(pid == 0){
                    if(execv(tuple[0], &tuple[1]) < 0){
                        fprintf(stderr, "line %d: execv failed: %s\n",__LINE__,strerror(errno));
                    }
                }
            }
            if((cmd_list->next == NULL) && ex){
                exit(EXIT_SUCCESS);
            }
            cmd_list = cmd_list->next;
        }
        //wait for parallel process
        wait(&num_process);

        mode = modenext;
    }
    free(tuple[1]);
    free(tuple[0]);
    free(tuple);
    free_allocs(head);
    return 0;
}
