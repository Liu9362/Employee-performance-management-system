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

#define MAX_SIZE 256

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

typedef struct _Node{
    char id[7];
    int fd;         
    struct _Node *next;
}node_t;

char c;

int insert_list(int acceptfd,  MSG *msg,node_t *phead);
int delete_list( int fd ,node_t *phead);

int do_client(int acceptfd, sqlite3 *dateb,node_t *phead);
int do_login(int acceptfd, MSG *msg, sqlite3 *dateb,node_t *phead);
int do_find(int acceptfd, MSG *msg, sqlite3 *dateb);
int do_revise(int acceptfd, MSG *msg, sqlite3 *dateb);
int do_add(int acceptfd, MSG *msg, sqlite3 *dateb);
int do_delete(int acceptfd, MSG *msg, sqlite3 *dateb);
#endif
