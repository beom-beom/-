#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 1024
#define NAME_SIZE 100
void *send_msg(void *arg);
void *recv_msg(void *arg);

char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];
int main(int argc,char *argv[])
{
            int sock;
            struct sockaddr_in serv_addr;
            pthread_t send,recv;
            void *thread_return;
            sprintf(name,"[%s]",argv[3]);
            sock=socket(PF_INET,SOCK_STREAM,0);
            memset(&serv_addr,0,sizeof(serv_addr));
            serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
            serv_addr.sin_port=htons(atoi(argv[2]));
            serv_addr.sin_family=AF_INET;
            if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1)
            {
                        fputs("connect error",stderr);
                        exit(1);
            }
            pthread_create(&send,NULL,send_msg,(void *)&sock);
            pthread_create(&recv,NULL,recv_msg,(void *)&sock);
            pthread_join(send,&thread_return);
            pthread_join(recv,&thread_return);
            close(sock);
            return 0;
}
void *send_msg(void *arg) {
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    while (1) {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "quit\n")) {
            close(sock);
            exit(1);
        }
        sprintf(name_msg, "%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
void *recv_msg(void *arg){
            int sock=*((int *)arg);
            char name_msg[NAME_SIZE+BUF_SIZE];
            int str_len;
            while(1){
                        str_len=read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);
                        if(str_len==-1)
                                    return (void *)-1;
                        name_msg[str_len]=0;
                        printf("%s", name_msg);
            }
            return NULL;
}