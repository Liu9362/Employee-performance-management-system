#include "project.h"

int aaa(int sockfd, MSG *msg)
{
    MSG msgtemp;
    msgtemp = *msg;
    int ret = 0;
    send(sockfd, &msgtemp, sizeof(MSG), 0);
    recv(sockfd, &msgtemp, sizeof(MSG), 0);
    if(!strncmp(msgtemp.data, "OK",2))
    {
        printf("输入修改内容：\n");
        scanf("%s", msgtemp.data);
        printf("要修改的：%s\n", msgtemp.data);
        send(sockfd, &msgtemp, sizeof(MSG), 0);
        recv(sockfd, &msgtemp, sizeof(MSG), 0);
        if (!strncmp(msgtemp.data, "OK",2))
        {
            printf("修改完成\n");
        }
        else
        {
            printf("修改用户修改信息未接收");
        }
    }
    else
    {
        strcpy(msgtemp.data,"error");
        send(sockfd, &msgtemp, sizeof(MSG), 0);
        printf("修改用户信息未接收");
    }
    return 0;
}

int  do_login(int sockfd, MSG *msg)
{
    int ret2 = 0;
    printf("请输入您的id:");
    scanf("%s", msg->name);
    printf("请输入您的密码:");
    scanf("%s", msg->data);
    ret2 = send(sockfd, msg, sizeof(MSG), 0);
    int ret = recv(sockfd, msg, sizeof(MSG), 0);
    if (strncmp(msg->data, "login OK", 8) == 0){

        system("clear");
        return 0;
    }else{
        printf("data [%s]\n", msg->data);
        return -1;
    } 
}

int login_ok(int sockfd, MSG *msg)
{   
    printf("登陆成功，正在加载系统，请稍后……\n");
    sleep(1);
    msg->choice = L;
    char ret = 'E';
    int num = 0;
    system("clear");
    if (msg->name[0]=='1')
    {
        ret = 'R';
        while (1)
        {
            printf("*****************欢迎使用,尊贵的管理员用户*******************\n");
            printf("***1:查看用户/更改用戶信息  2:增加新用户 3:删除用户 0:退出 ***\n");
            printf("***********************************************************\n");
            printf("请输入您想要进行的操作 : ");
            scanf("%d", &num);
            switch (num)
            {
            case 1:
                msg->choice = F;
                do_find(sockfd, msg, ret);
                break;
            case 2:
                msg->choice = A;
                do_add(sockfd, msg);
                break;
            case 3:
                msg->choice = D;
                do_delete(sockfd, msg);
                break;
            case 0:
                return 0;
            default:
                printf("选项错误，请重试\n");
            }
        }
    }
    else if (msg->name[0] == '0')
    {
        while (1)
        {
            printf("*************尊敬的[%s]，欢迎使用员工系统**************\n", msg->name);
            printf("*************1:查看信息   2:修改信息   0:退出 ***************\n");
            printf("***********************************************************\n");
            printf("请输入您想要进行的操作 : ");
            scanf("%d", &num);
            switch (num)
            {
            case 1:
                msg->choice = F;
                do_find(sockfd, msg, ret);
                break;
            case 2:
                msg->choice = R;
                do_revise(sockfd, msg);
                break;
            case 0:
                system("clear");
                return 0;
            default:
                printf("选项错误，请重试\n");
            }
        }
    }
}

int do_find(int sockfd, MSG *msg, char tpe)
{   

    printf("do_find\n");
    msg->choice=F;
    char set[7]={0};
    char temp_id[7]={0};
    char a[5] = {0};
    int ret = 0;
    printf("find msg name[%s]name[0[%c]]\n",msg->name,msg->name[0]);
    if(msg->name[0]== '1'){
        printf("==R\n");
        tpe='R';
    }
    system("clear"); 
    sprintf(msg->data, "%c", tpe);

    printf("msgdata[%s]\n",msg->data);
    send(sockfd, msg, sizeof(MSG), 0);
    recv(sockfd, msg, sizeof(MSG), 0);
    printf("%s\n", msg->data);

    if (tpe == 'R')
    {
        while (1)
        {
            if (ret == 1)
            {
                printf("输入@@@@修改当前用户信息");
            }
            printf("请输入目标的id\t输入####返回\n");
            scanf("%s", set);
            msg->choice = F;
            if (!(strncmp(set, "####", 4)))
            {
                system("clear");
                return 0;
            }
            else if (!strncmp(set, "@@@@", 4))
            {
                strcpy(msg->data,temp_id);
                do_revise_root(sockfd, msg);
                
                printf("@@@error:choice:[%d]name[%s]data[%s]\n",msg->choice,msg->name,msg->data);
                return 0;
            }
            else
            {   

                strcpy(msg->data,set);
                strcpy(temp_id,set);
                send(sockfd, msg, sizeof(MSG), 0);
                recv(sockfd, msg, sizeof(MSG), 0);
                if (!strcmp(msg->data, "没有该员工!!!"))
                {
                    printf("%s\n", msg->data);
                }
                else
                {
                    ret = 1;
                    printf("%s\n%s\n--------------------------\n", msg->name, msg->data);
                }
            }
        }
    }
    return 0;
}

int do_revise_root(int sockfd, MSG *msg)
{   
    int a;

    msg->choice=R;
    send(sockfd,msg,sizeof(MSG),0);
    recv(sockfd, msg, sizeof(MSG), 0);
    if(!strcmp(msg->data, "ac_id ok")){
         printf("目标id确认接收[%s]\n",msg->data);
    }else{
        strcpy(msg->data,"error");
        send(sockfd,msg,sizeof(MSG),0);
        return 0;
    }
 
        printf("*********************输入您要修改的选项**********************\n");
        printf("1:修改姓名 2:修改职务 3:修改联系方式 4:修改本月薪水 5:修改密码 0:退出\n");
        printf("***********************************************************\n");
        printf("请输入您想要进行的操作 : \n");
        scanf("%d", &a);
        switch (a)
        {
        case 1:
            strcpy(msg->data, "a");
            aaa(sockfd, msg);
            break;
        case 2:
            strcpy(msg->data, "b");
            aaa(sockfd, msg);
            break;
        case 3:
            strcpy(msg->data, "c");
            aaa(sockfd, msg);
            break;
        case 4:
            strcpy(msg->data, "d");
            aaa(sockfd, msg);
            break;
        case 5:
            strcpy(msg->data, "e");
            aaa(sockfd, msg);
            break;
        case 0:
            msg->choice=F;
            system("clear");
            return 0;
        default:
            printf("选项错误\n");
        }

}

int do_revise(int sockfd, MSG *msg)
{
    int  a;
    msg->choice=R;

    system("clear");
    while(1){
        printf("*********************输入您要修改的选项************************\n");
        printf("*******1:修改姓名  2:修改联系方式  3:修改密码  0:退出 **********\n");
        printf("*************************************************************\n");
        printf("请输入您想要进行的操作 : ");
        printf("xiugai scanf %c\n",c);
       while(0==scanf("%d",&a));
            switch (a){
                case 1:
                    strcpy(msg->data,"a");
                    aaa(sockfd,msg);
                    break;
                case 2:
                    strcpy(msg->data,"c");
                    aaa(sockfd,msg);
                    break;
                case 3:
                    strcpy(msg->data,"e");
                    aaa(sockfd,msg);
                    break;
                case 0:
                    return 0;
                    system("clear");
                default:
                    printf("修改选项错误\n");
            }
    }
    return 0;
}

int do_add(int sockfd, MSG *msg)
{
    printf("do_add\n");

    msg->choice = A;
    int ret=0;
    int root;
    printf("请输入添加员工的姓名：");
    scanf("%s",msg->data);
    strcat(msg->data,"/");
    printf("name %s\n",msg->name);
RE:
    printf("请输入添加员工的权限（1为经理，0为员工）：\n");
    scanf("%d",&root);
    switch(root){
        case 1:
            strcat(msg->data,"1");
            break;
        case 0:
            strcat(msg->data,"0");
            break;
        default:
            printf("权限不合规，已返回上一级\n");
            goto RE;
    }
    printf("data %s\n",msg->data);
    send(sockfd,msg,sizeof(MSG),0);
    if(0>=(ret=recv(sockfd,msg,sizeof(MSG),0))){
        ERR_LOG("出错了，添加失败");
        return -1;
    }
    if(!strncmp(msg->data,"OK",2)){
        printf("添加成功\n请输入联系方式：");
        memset(msg->data,0,sizeof(msg->data));
        scanf("%s",msg->data);
        send(sockfd,msg,sizeof(MSG),0);
        if(0>=(ret=recv(sockfd,msg,sizeof(MSG),0))){
            ERR_LOG("出错了，电话添加失败");
            return -1;
        }else if(strncmp(msg->data,"OK",2)){
            printf("添加成功\n");
            return 0;
        }

    }
        printf("添加失败,已返回上一级\n");
        return -1;
}

int do_delete(int sockfd, MSG *msg)
{
    printf("do_delete\n");
    msg->choice = D;
    int ret=0;
    printf("请输入删除员工的id：");
    scanf("%s",msg->data);
    send(sockfd,msg,sizeof(MSG),0);
    if(0>=(ret=recv(sockfd,msg,sizeof(MSG),0))){
        ERR_LOG("出错了，删除失败");
        return -1;
    }
    if(strncmp(msg->data,"OK",2)){
        printf("删除成功\n");
            return 0;
    }
        printf("删除失败,已返回上一级\n");
        return -1;
}