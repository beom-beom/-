#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define BUF_SIZE 1024
#define MAX_CLNT 100

void send_msg(char *msg,int len);
void *handle_clnt(void *arg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mtx;
int main(int argc,char *argv[])
{
            int serv,clnt;
            struct sockaddr_in serv_addr,clnt_addr;
            socklen_t addr_size;
            pthread_t t_id;
            pthread_mutex_init(&mtx,NULL);
            serv=socket(PF_INET,SOCK_STREAM,0);
            memset(&serv_addr,0,sizeof(serv_addr));
            serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
            serv_addr.sin_family=AF_INET;
            serv_addr.sin_port=htons(atoi(argv[1]));
            if(bind(serv,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1)
            {
                        fputs("bind error",stderr);
                        exit(1);
            }
            if(listen(serv,5)==-1)
            {
                        fputs("listen error",stderr);
                        exit(1);
            }
            while(1){
            addr_size=sizeof(clnt_addr);
            clnt=accept(serv,(struct sockaddr *)&clnt_addr,&addr_size);
            pthread_mutex_lock(&mtx);
            clnt_socks[clnt_cnt++]=clnt;
            pthread_mutex_unlock(&mtx);
            pthread_create(&t_id,NULL,handle_clnt,(void *)&clnt);
            pthread_detach(t_id);
            printf("connected client Ip: %s \n",inet_ntoa(clnt_addr.sin_addr));

            }
            close(serv);
            return 0;
}
void *handle_clnt(void *arg){
            int clnt=*((int *)arg);
            int str_len=0;
            char buf[BUF_SIZE];
            while((str_len=read(clnt,buf,sizeof(buf)))!=0)
            {
                        send_msg(buf,str_len);
            }
            pthread_mutex_lock(&mtx);
            for(int i=0;i<clnt_cnt;i++)
            {
            if(clnt==clnt_socks[i])
            {
                        while(i++<clnt_cnt-1)
                        {
                                    clnt_socks[i]=clnt_socks[i+1];
                        }
                        break;
            }
            }
            clnt_cnt--;
            pthread_mutex_unlock(&mtx);
            close(clnt);
            return NULL;
}
void send_msg(char *msg,int len)
{
            pthread_mutex_lock(&mtx);
            for(int i=0;i<clnt_cnt;i++)
            {
                        write(clnt_socks[i],msg,len);
            }
            pthread_mutex_unlock(&mtx);
}