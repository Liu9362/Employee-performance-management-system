#include "project.h"

int insert_list(int acceptfd, MSG *msg, node_t *phead)
{
    if (NULL == phead)
    {
        printf("入参为NULL\n");
        return -1;
    }
    node_t *newnote = phead;
    while (newnote != NULL)
    {
        printf("while\n");
        if (!strcmp(newnote->id, msg->name))
        {
            return -1;
        }
        newnote = newnote->next;
    }
    newnote = (node_t *)malloc(sizeof(node_t));
    if (NULL == newnote)
    {
        printf("分配地址失败\n");
        return -1;
    }
    newnote->next = phead->next;
    phead->next = newnote;
    newnote->fd=acceptfd;
    strcpy(newnote->id, msg->name);
    return 0;
}

int delete_list(int fd, node_t *phead)
{
    if (NULL == phead)
    {
        printf("入参为NULL\n");
        return -1;
    }
    if (NULL == phead->next)
    {
        printf("表空 删除失败\n");
        return -1;
    }
    printf("delete\n");

    node_t *tempnote = phead;
    
    node_t *delnote = tempnote->next;
    while (delnote != NULL)
    {
        if (delnote->fd==fd)
        {
            tempnote->next = delnote->next;
            printf("free 前\n");
            free(delnote);
            printf("free 后\n");
            delnote = NULL;
            printf("下机目标id[%s]\n",delnote->id);
            break;
        }
        tempnote = tempnote->next;
        delnote = tempnote->next;
    }
    return 0;
}

int do_client(int acceptfd, sqlite3 *dateb, node_t *phead)
{

    MSG msg;
    MSG tempmsg;
    int ret = 0;
    ret = recv(acceptfd, &msg, sizeof(MSG), 0);
    tempmsg=msg;
    if (0 < ret)
    {
        printf("choice:[%d]", msg.choice);
        switch (msg.choice)
        {
        case L:
            do_login(acceptfd, &msg, dateb, phead);
            break;
        case F:
            do_find(acceptfd, &msg, dateb);
            break;
        case R:
            do_revise(acceptfd, &msg, dateb);
            break;
        case A:
            do_add(acceptfd, &msg, dateb);
            break;
        case D:
            do_delete(acceptfd, &msg, dateb);
            delete_list(acceptfd, phead);
            printf("%s用户退出\n",msg.name);
            return 0;
        default:
            printf("选项错误，请重试\n");
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
    else if (ret = 0)
    {
        delete_list(acceptfd, phead);
        printf("%s客户端关闭\n",msg.name);
        return -1;
    }
    else
    {
        delete_list(acceptfd, phead);
        printf("%s接收错误\n",msg.name);
        ERR_LOG("接收错误");
    }
    return 0;
}

int do_login(int acceptfd, MSG *msg, sqlite3 *dateb, node_t *phead)
{

    // printf("%s\n%s\twuhuaaa\n",msg->name,msg->data);
    char sqlstr[2048] = {0};
    char *errmsg = NULL, **result = NULL;
    int nrow = 0, ncolumn = 0;
    int ret = 0;
    sprintf(sqlstr, "select root from passwd where id='%s'and pass='%s'", msg->name, msg->data);
    if (SQLITE_OK != sqlite3_get_table(dateb, sqlstr, &result, &nrow, &ncolumn, &errmsg))
    {
        printf("error : %s\n", errmsg);
    }
    sqlite3_free(errmsg);
    
    if (nrow == 0)
    {   
        printf("%s尝试登录，登陆失败\n",msg->name);
        strcpy(msg->data, "name or password is wrony!!!");
    }
    else
    {
        ret = insert_list(acceptfd, msg, phead);
        if (ret == -1)
        {
            sprintf(msg->data, "重复登陆\n");
            send(acceptfd, msg, sizeof(MSG), 0);
            printf("%s重复登录\n",msg->name);
            return 0;
        }
        strcpy(msg->data, "login OK\0");
        if (!strncmp(result[1], "1", 1))
        {
            msg->choice = '1';
        }
        else
        {
            msg->choice = '0';
        }
    }
    send(acceptfd, msg, sizeof(MSG), 0);
    printf("%s登录成功\n", msg->name);
    return 0;
}
int do_find(int acceptfd, MSG *msg, sqlite3 *dateb)
{
    printf("do find\n");
    char sqlstr[512] = {0};
    char *errmsg = NULL, **result = NULL;
    int nrow = 0, ncolumn = 0;
    int i = 0;
    char ac_id[10] = {0};
    printf("%s查找数据\n",msg->name);
    if (0 == strncmp(msg->data, "R", 1))
    {
        sprintf(sqlstr, "select * from passwd");
        if (SQLITE_OK != sqlite3_get_table(dateb, sqlstr, &result, &nrow, &ncolumn, &errmsg))
        {
            printf("error : %s\n", errmsg);
        }
        sqlite3_free(errmsg);
        memset(msg->data, 0, sizeof(msg->data));
        for (i = 1; i <= nrow; i++)
        {
            sprintf(msg->data, "%s%s\t%s\n", msg->data, result[i * 4], result[i * 4 + 1]);
        }
        if (-1 == send(acceptfd, msg, sizeof(MSG), 0))
            ERR_LOG("send error");
        printf("[%s]打印以下数据\n",msg->name);
        printf("%s\n",msg->data);
        printf("%s查询结果已发送\n",msg->name);
    }
    else
    {
        if (!strncmp(msg->name, "1", 1))
        {
            strcpy(ac_id, msg->data);
            printf("%s\n",msg->data);
        }
        else
        {
            strcpy(ac_id, msg->name);
        }
        printf("ac id [%s]\n",ac_id);
        sprintf(sqlstr, "select * from name where id='%s'",ac_id );
        if (SQLITE_OK != sqlite3_get_table(dateb, sqlstr, &result, &nrow, &ncolumn, &errmsg))
        {
            printf("error : %s\n", errmsg);
        }
        sqlite3_free(errmsg);
        // printf("result [%c]\n",result[1][0]);
        if (nrow == 0)
        {
            strcpy(msg->data, "没有该员工!!!");
        }
        else
        {
            memset(msg->data, 0, sizeof(msg->data));
            sprintf(msg->data, "姓名：%s\t职务：%s\n联系方式：%s\n本月薪水：%s", result[7], result[8], result[9], result[10]);
        }
        printf("[%s]打印以下数据\n",msg->name);
        printf("%s\n",msg->data);
        printf("%s查询结果已发送\n",msg->name);
        if (-1 == send(acceptfd, msg, sizeof(MSG), 0))
            ERR_LOG("send error");
    }
    return 0;
}
int do_revise(int acceptfd, MSG *msg, sqlite3 *dateb)
{
    printf("%s修改数据\n",msg->name);

    // char usrid[7]={0};
    char datetemp[10]={0};
    char sqlstr[3096] = {0};
    char sqlbuff[2048] = {0};
    int ret = 0;
    char ac_id[7] = {0};
    char ac_set[10] = {0};
    char **result = NULL;
    if (msg->name[0]=='1' )
    {
        strcpy(ac_id, msg->data);
        strcpy(msg->data, "ac_id ok");
        send(acceptfd, msg, sizeof(MSG), 0);
        printf("目标id[%s]\n",msg->data);
        recv(acceptfd, msg, sizeof(MSG), 0);
        if(!strcmp(msg->data,"error"))
            ERR_LOG("修改ack未接受");
    }
    else
    {
        strcpy(ac_id, msg->name);
    }
    strcpy(ac_set, msg->data);
    if (!strncmp(ac_set, "a", 1))
    {
        strcpy(ac_set, "name");
    }
    else if (!strncmp(ac_set, "b", 1))
    {
        strcpy(ac_set, "post");
    }
    else if (!strncmp(ac_set, "c", 1))
    {
        strcpy(ac_set, "num");
    }
    else if (!strncmp(ac_set, "d", 1))
    {
        strcpy(ac_set, "salary");
    }
    else if (!strncmp(ac_set, "e", 1))
    {
        strcpy(ac_set, "pass");
    }
    strcpy(msg->data, "OK\0");
    if (-1 == send(acceptfd, msg, sizeof(MSG), 0))
        ERR_LOG("send error");
    if (-1 == recv(acceptfd, msg, sizeof(MSG), 0))
        ERR_LOG("recv error");
    printf("修改id[%s]--[%s]%s被修改数据:[%s]\n",msg->name,ac_id,ac_set,msg->data);
    if(!strcmp(msg->data,"error")){
        printf("[%s]修改失败\n",msg->name);
        return 0;
    }
    if (!strncmp(ac_set, "pass", 4))
    {
        sprintf(sqlstr, "UPDATE passwd SET %s='%s' WHERE id='%s'", ac_set, msg->data, ac_id);
        if (SQLITE_OK != (ret = sqlite3_get_table(dateb, sqlstr, &result, NULL, NULL, NULL)))
        {
            printf("[%s]修改失败\n",msg->name);
            return -1;
        }
    }
    else if (!strncmp(ac_set, "name", 4))
    {

        printf("pass name zhe\n");
        sprintf(sqlstr, "UPDATE passwd SET name='%s' WHERE id='%s'", msg->data, ac_id);
        printf("passwd %s\n", sqlstr);
        if (SQLITE_OK != (ret = sqlite3_get_table(dateb, sqlstr, &result, NULL, NULL, NULL)))
        {
            printf("[%s]修改失败\n",msg->name);
            return -1;
        }
        sprintf(sqlstr, "UPDATE name SET name='%s' WHERE id='%s'", msg->data, ac_id);
        printf("name %s\n", sqlstr);
        if (SQLITE_OK != (ret = sqlite3_get_table(dateb, sqlstr, &result, NULL, NULL, NULL)))
        {
            printf("[%s]修改失败\n",msg->name);
            return -1;
        }
    }
    else
    {
        sprintf(sqlstr, "UPDATE name SET %s='%s' WHERE id='%s'", ac_set, msg->data, ac_id);
        if (SQLITE_OK != (ret = sqlite3_get_table(dateb, sqlstr, &result, NULL, NULL, NULL)))
        {
            printf("[%s]修改失败\n",msg->name);
            return -1;
        }
    }
    sprintf(msg->data, "OK");
    if (-1 == send(acceptfd, msg, sizeof(MSG), 0))
        ERR_LOG("send error");
    printf("%s修改成功\n",msg->name);
    return 0;
}
int do_add(int acceptfd, MSG *msg, sqlite3 *dateb)
{   
    char sqlstr[2048] = {0};
    char *errmsg = NULL, **result = NULL;
    int nrow = 0, ncolumn = 0;
    char buff_name[20]={0};
    char buff_id[64]={0};
    char buff_post[64]={0};
    char buff_root[10]={0};
    int ret=0;
    int i=0;
    // MSG msgtemp;
    while(1){
        if(msg->data[i]=='/'){
            msg->data[i]='\0';
            break;
        }
        i++;
    }
    strcpy(buff_name,msg->data);
    // msgtemp=*msg;
    
    // printf("msg name %s\n",msg->name);
    // printf("msg data %s\n",msg->data);
    if(msg->data[i+1]=='1'){
        sprintf(buff_post,"经理");
        sprintf(buff_root,"1");
    }else if(msg->data[i+1]=='0'){
        sprintf(buff_post,"员工");
        sprintf(buff_root,"0");
    }
    sprintf(sqlstr, "select id from passwd where root='%s'",buff_root);
    if(SQLITE_OK != sqlite3_get_table(dateb, sqlstr, &result, &nrow, &ncolumn, &errmsg)){
    	printf("error : %s\n", errmsg);
    }
    printf("%d\n",nrow);
    strncpy(buff_id,result[nrow],6);
    printf("111111\n%s\n",result[nrow-1]);
    buff_id[5]++;
    printf("111111\n");
    sprintf(sqlstr, "INSERT INTO passwd VALUES('%s','%s','%s','%s')",buff_id,buff_name,buff_id,buff_root);
    if(SQLITE_OK != (ret = sqlite3_exec(dateb, sqlstr, NULL, NULL, NULL))){
        printf("信息添加失败 : [%d]:[%s]\n", ret, sqlite3_errmsg(dateb));
        printf("%s添加用户失败\n",msg->name);
        return -1;
    }else{
        memset(msg->data,0,sizeof(msg->data));
        sprintf(msg->data,"OK");
    }
    if(0>=(send(acceptfd,msg,sizeof(MSG),0)))
        ERR_LOG("add send error");
    if(0>=(recv(acceptfd,msg,sizeof(MSG),0)))
        ERR_LOG("add1 recv error");

    sprintf(sqlstr, "INSERT INTO name VALUES('%s','%s','%s','%s','0','%s')",buff_id,buff_name,buff_post,msg->data,buff_root);
    if(SQLITE_OK != (ret = sqlite3_exec(dateb, sqlstr, NULL, NULL, NULL))){
        printf("信息添加失败 : [%d]:[%s]\n", ret, sqlite3_errmsg(dateb));
        printf("%s添加用户失败\n",msg->name);
        return -1;
    }else{
    if(0>=(send(acceptfd,msg,sizeof(MSG),0)))
        ERR_LOG("add send error");
    }
    memset(msg->data,0,sizeof(msg->data));
    sprintf(msg->data,"OK\n");
    if(0>=(send(acceptfd,msg,sizeof(MSG),0)))
            ERR_LOG("add send error");
    printf("%s新加用户:[%s][%s][%s][%s]\n",msg->name,buff_id,buff_name,buff_root,buff_post);
    return 0;
}

int do_delete(int acceptfd, MSG *msg, sqlite3 *dateb)
{
        int ret = 0;
        char sqlbuff[2048] = {0};
        sprintf(sqlbuff, "DELETE FROM passwd WHERE id='%s'", msg->data);

        if(SQLITE_OK != (ret = sqlite3_exec(dateb, sqlbuff, NULL, NULL, NULL))){
            printf("账户删除失败 : [%d]:[%s]\n", ret, sqlite3_errmsg(dateb));
            printf("%s删除用户失败\n",msg->name);
            goto err;
        }
        sprintf(sqlbuff, "DELETE FROM employees WHERE id='%s'", msg->data);
        if(SQLITE_OK != (ret = sqlite3_exec(dateb, sqlbuff, NULL, NULL, NULL))){
            printf("信息删除失败 : [%d]:[%s]\n", ret, sqlite3_errmsg(dateb));
            printf("%s删除用户失败\n",msg->name);
            goto err;
        }
        memset(msg->data,0,sizeof(msg->data));
        sprintf(msg->data,"OK");
        if(0>=(send(acceptfd,msg,sizeof(MSG),0)))
                ERR_LOG("add send error");
                printf("%s添加用户%s成功\n",msg->name,msg->data);
        return 0;
    err:
        memset(msg->data,0,sizeof(msg->data));
        sprintf(msg->data,"删除失败\n");
        if(0>=(send(acceptfd,msg,sizeof(MSG),0)))
                ERR_LOG("add send error");
    return -1;
}
