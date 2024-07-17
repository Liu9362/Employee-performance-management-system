#include "project.h"

int main(int argc, char const *argv[])
{
    sqlite3 *dateb = NULL;

    // if (argc < 3)
    // {
    //     printf("Usage : %s <serv_ip> <serv_port>\n", argv[0]);
    //     exit(-1);
    // }

    int ret = 0;
    if (SQLITE_OK != (ret = sqlite3_open("employees.db", &dateb)))
    {
        printf("打开数据库文件失败 : [%d]:[%s]\n", ret, sqlite3_errmsg(dateb));
        exit(EXIT_FAILURE);
    }
    printf("数据库文件打开成功..\n");

    node_t *phead = NULL;
    phead = (node_t *)malloc(sizeof(node_t));
    if(NULL == phead){
        printf("内存分配失败\n");
        return -1;
    }
    strcat(phead->id,"id");
    phead->next = NULL;


    int sockfd = 0;
    if (0 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
        ERR_LOG("socket error");

    struct sockaddr_in serveraddr;
    serveraddr.sin_addr.s_addr = inet_addr("192.168.100.10");
    serveraddr.sin_port = htons(36777);
    serveraddr.sin_family = AF_INET;
    socklen_t serveraddr_len = sizeof(serveraddr);

    if (-1 == (bind(sockfd, (struct sockaddr *)&serveraddr, serveraddr_len)))
        ERR_LOG("bind error");

    if (-1 == (listen(sockfd, 10)))
        ERR_LOG("listen error");
    int i = 0;
    int num = 0;
    int epfd = 0;
    int fd = 0;
    if (0 > (epfd = epoll_create(1)))
        ERR_LOG("epoll error");
    struct epoll_event ep_ev;
    struct epoll_event events[MAX_SIZE];
    ep_ev.data.fd = sockfd;
    ep_ev.events = EPOLLIN;
    if (0 > (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ep_ev)))
        ERR_LOG("epoll ctl error");
    while (1)
    {
        num = epoll_wait(epfd, events, MAX_SIZE, -1);

        for (i = 0; i < num; i++)
        {
            printf("jj");
            // 接受客户端的连接请求
            if (EPOLLIN & events[i].events)
            {
                if (events[i].data.fd == sockfd)
                {
                    printf("新连接\n");
                    int clientfd = accept(sockfd, NULL, NULL);
                    if (-1 == (clientfd))
                        ERR_LOG("accept error");

                    ep_ev.data.fd = clientfd;
                    ep_ev.events = EPOLLIN;
                    if (0 > epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ep_ev))
                    {
                        ERR_LOG("epoll ctl error");
                        close(clientfd);
                    }
                }
                else
                {
                    printf("客户端数据\n");
                  
                    ret = do_client(events[i].data.fd, dateb,phead);
                    if (ret == -1)
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ep_ev);
                        close(events[i].data.fd);
                    }
                }
            }
        }
    }
    return 0;
}
