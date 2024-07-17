#ifndef __PROJECT_H__
#define __PROJECT_H__
#include<stdio.h>
#include<error.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sqlite3.h>
#include<unistd.h>

#define L 1
#define F 2
#define R 3
#define A 4
#define D 5
#define ERR_LOG(msg) do{\
    printf("%s :%s :%d\n",__FILE__,__func__,__LINE__);  \
    perror(msg);  \
    return -1;  \
}while(0);  

typedef struct {
    int choice;
    char name[64];
    char data[1024];

}MSG;

char c;

int  do_login(int sockfd,MSG *msg);
int login_ok(int sockfd,MSG *msg);
int do_find(int sockfd,MSG *msg,char ret);
int do_revise_root(int sockfd,MSG *msg);
int do_revise(int sockfd,MSG *msg);
int do_add(int sockfd,MSG *msg);
int do_delete(int sockfd,MSG *msg);
#endif