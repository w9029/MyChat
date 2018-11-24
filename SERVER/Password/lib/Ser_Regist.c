#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern struct Linker Linker_head;	//在线人链表头
extern struct Linker *Last_Linker;	//指向最后一个在线人
extern int sfd;
extern MYSQL *mysql;

void Ser_Regist(int cfd)
{
	int ret;
	struct MSG msgbuf;
	struct Linker *temp;
	char username[50], passwd[50], question[50], answer[50];
	char query[100];
	printf("Login\n");
	MYSQL_ROW row;
	MYSQL_RES *re;

	printf("Regist\n");
	//Get Username
	while(1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret <= 0)
		{
			perror("recv Regist error\n");
			break;
		}
		if(msgbuf.MSG_TYPE != MSG_REGIST)
		{
			printf("recv a wrong MSG when Regist\n");
			return;
		}
		strcpy(username, msgbuf.MSG_data);
		if(strcmp(username, "0") == 0)
		{
			printf("username give up\n");
			return;
		}
		printf("username:%s ",username);

		//从数据库中查询
		sprintf(query, "select username from users where username = '%s'",username);
		mysql_real_query(mysql, query, strlen(query));
		re = mysql_store_result(mysql);
		//判断是否重名
		row = mysql_fetch_row(re);
		if(row == NULL)
		{
			SendAck(cfd);
			printf("-Success\n");
			break;
		}
		else
		{
			SendFail(cfd, "This name is used");
			printf("-Used name\n");
		}
	}

	//Get Passwd
	bzero(&msgbuf, sizeof(struct MSG));
	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret <= 0)
	{
		perror("recv Regist error\n");
	}
	if(msgbuf.MSG_TYPE != MSG_REGIST)
	{
		printf("recv a wrong MSG when Regist\n");
		return;
	}
	strcpy(passwd, msgbuf.MSG_data);
	if(strcmp(passwd, "0") == 0)
	{
		printf("passwd give up\n");
		return;
	}
	printf("passwd:%s \n", passwd);
	SendAck(cfd);

	//Get Question
	bzero(&msgbuf, sizeof(struct MSG));
	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret <= 0)
	{
		perror("recv Regist error\n");
	}
	if(msgbuf.MSG_TYPE != MSG_REGIST)
	{
		printf("recv a wrong MSG when Regist\n");
		return;
	}
	strcpy(question, msgbuf.MSG_data);
	if(strcmp(question, "0") == 0)
	{
		printf("question give up\n");
		return;
	}
	printf("question:%s \n", question);
	SendAck(cfd);

	//Get Answer
	bzero(&msgbuf, sizeof(struct MSG));
	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret <= 0)
	{
		perror("recv Regist error\n");
	}
	if(msgbuf.MSG_TYPE != MSG_REGIST)
	{
		printf("recv a wrong MSG when Regist\n");
		return;
	}
	strcpy(answer, msgbuf.MSG_data);
	if(strcmp(answer, "0") == 0)
	{
		printf("answer give up\n");
		return;
	}
	printf("answer:%s \n", answer);
	SendAck(cfd);

	//加入数据库
	sprintf(query, "insert into users values ('%s','%s','%s','%s')",
			username, passwd, question, answer);
	ret = mysql_real_query(mysql, query, strlen(query));
	if(ret)
	{
		printf("SQL insert error\n");
		SendFail(cfd, "SQL insert error");
		return;
	}
	else
	{
		SendAck(cfd);
	}

	//注册成功
	printf("Regist success\n");
}


