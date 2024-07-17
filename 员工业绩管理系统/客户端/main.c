#include "project.h"
int main(int argc, char const *argv[])
{
    MSG usrmsg;
    char my_ip[17];
    char my_port[6];
    printf("ip:");
    scanf("%s",my_ip);
    printf("\nport:");
    scanf("%s",my_port);
    printf("\n");
    
    int sockfd=0;
    if(0==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
        ERR_LOG("socket error");
    
    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr=inet_addr(my_ip);
    server_addr.sin_port=htons(atoi(my_port));
    server_addr.sin_family=AF_INET;
    socklen_t server_addr_len=sizeof(server_addr);

    if(-1==connect(sockfd,(struct sockaddr *)&server_addr,server_addr_len))
        ERR_LOG("connect error");
    MSG msg;
    int choice=0;
    while(1){
        printf("************************************\n");
		printf("*********1:登录   0: 退出 ***********\n");
		printf("************************************\n");
		printf("please input choice : ");
        scanf("%d",&choice);
        switch (choice){
            case 1:
                msg.choice=L;
                if(!do_login(sockfd,&msg))
                login_ok(sockfd, &msg);
                break;
            case 0:
                close(sockfd);
                printf("///欢迎下次使用///\n");
                sleep(1);
                system("clear");
                exit(0);
            default:
                printf("选项错误，请重试\n");
                while((c= getchar())!='\n'&& c != EOF);
        }
    }
    return 0;
}