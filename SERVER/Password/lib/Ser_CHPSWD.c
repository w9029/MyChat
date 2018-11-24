#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Get_user_stat.h"

extern struct Linker Linker_head;	//在线人链表头
extern struct Linker *Last_Linker;	//指向最后一个在线人
extern int sfd;
extern MYSQL *mysql;

void Ser_CHPSWD(int cfd)
{
	int ret, flag;
	struct MSG msgbuf;
	char Ch_Type[20], username[50], passwd[50], sql_passwd[50];
	char query[100];
	printf("CHPSWD\n");
	MYSQL_ROW row;
	MYSQL_RES *re;

	//Get Username
	while(1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret <= 0)
		{
			perror("recv Login error\n");
			break;
		}
		if(msgbuf.MSG_TYPE != MSG_CHPSWD)
		{
			printf("recv a wrong MSG when CHPSWD\n");
			return;
		}
		strcpy(username, msgbuf.MSG_data);
		if(strcmp(username, "0") == 0)
		{
			printf("username give up\n");
			return;
		}
		printf("CHPSWD username:%s ",username);

		//从数据库中查询
		sprintf(query, "select username from users where username = '%s'",username);
		mysql_real_query(mysql, query, strlen(query));
		re = mysql_store_result(mysql);
		//判断查询结果
		row = mysql_fetch_row(re);
		if(row == NULL)
		{
			SendFail(cfd, "Unknown Username");
			printf("-Unknown Username\n");
		}
		else
		{
			SendAck(cfd);
			printf("-Success\n");
			break;
		}
	}

	//接收消息 确认以哪一种方式更改密码
	flag = 0;
	while(flag == 0)
	{
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret <= 0)
		{
			perror("recv CHPSWD error\n");
			break;
		}
		if(msgbuf.MSG_TYPE != MSG_CHPSWD)
		{
			printf("recv a wrong MSG when CHPSWD\n");
			return;
		}
		strcpy(Ch_Type, msgbuf.MSG_data);
		if(strcmp(Ch_Type, "Ch_by_PW") == 0)
		{
			//进入密码更改函数
			flag = Ch_by_PW(cfd, username);
		}
		else if(strcmp(Ch_Type, "Ch_by_Q") == 0)
		{
			//进入密保更改函数
			flag = Ch_by_Q(cfd, username);
		}
		else if(strcmp(Ch_Type, "give up CHPSWD") == 0)
		{
			//放弃更改
			printf("%s CHPSWD give up\n", username);
			return;
		}
	}

	//修改成功
	printf("%s CHPSWD success\n", username);
}

int Ch_by_PW(int cfd, char username[])
{
	int ret, flag;
	struct MSG msgbuf;
	char passwd[50], sql_passwd[50];
	char query[100];
	printf("CHPSWD by PW\n");
	MYSQL_ROW row;
	MYSQL_RES *re;

	//查询密码
	sprintf(query, "select passwd from users where username = '%s'",username);
	mysql_real_query(mysql, query, strlen(query));
	re = mysql_store_result(mysql);
	row = mysql_fetch_row(re);
	strcpy(sql_passwd, row[0]);

	//Get Passwd
	while(1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret <= 0)
		{
			perror("Ch_by_PW recv passwd error\n");
			break;
		}
		if(msgbuf.MSG_TYPE != MSG_CHPSWD)
		{
			printf("recv a wrong MSG when Ch_by_PW\n");
			return 0;
		}
		strcpy(passwd, msgbuf.MSG_data);
		if(strcmp(passwd, "0") == 0)
		{
			printf("CHPSWD passwd give up\n");
			return 0;
		}

		printf("passwd:%s ", passwd);
		if(strcmp(passwd, sql_passwd) == 0)
		{
			//密码正确 判断用户是否在线
			flag = Get_user_stat(username);
			if(flag == 0)
			{
				SendAck(cfd);
				printf("-Success\n");
				break;
			}
			else
			{
				SendFail(cfd, "this user is on line");
				printf("-The user is on line\n");
			}
		}
		else 
		{
			SendFail(cfd, "Wrong passwd");
			printf("-Wrong Passwd\n");
		}
	}

	//调用更改函数
	ret = Get_Set_newpasswd(cfd, username);
	return ret;

}

int Ch_by_Q(int cfd, char username[])
{
	int ret, flag;
	struct MSG msgbuf;
	char sql_question[50], answer[50], sql_answer[50];
	char query[100];
	printf("CHPSWD by question\n");
	MYSQL_ROW row;
	MYSQL_RES *re;

	//查询密保问题并发送
	sprintf(query, "select question from users where username = '%s'",username);
	mysql_real_query(mysql, query, strlen(query));
	re = mysql_store_result(mysql);
	row = mysql_fetch_row(re);
	strcpy(sql_question, row[0]);

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_CHPSWD;
	strcpy(msgbuf.MSG_data, sql_question);
	ret = send(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret < 0)
	{
		perror("send question error\n");
	}

	//查询问题答案
	sprintf(query, "select answer from users where username = '%s'",username);
	mysql_real_query(mysql, query, strlen(query));
	re = mysql_store_result(mysql);
	row = mysql_fetch_row(re);
	strcpy(sql_answer, row[0]);

	//接收问题答案 并与正确答案进行比对
	while(1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret <= 0)
		{
			perror("recv answer error\n");
			break;
		}
		if(msgbuf.MSG_TYPE != MSG_CHPSWD)
		{
			printf("recv a wrong MSG when Login\n");
			return 0;
		}
		strcpy(answer, msgbuf.MSG_data);
		if(strcmp(answer, "0") == 0)
		{
			printf("CHPSWD passwd give up\n");
			return 0;
		}

		printf("answer:%s ", answer);
		if(strcmp(answer, sql_answer) == 0)
		{
			//问题通过 判断用户是否在线
			flag = Get_user_stat(username);
			if(flag == 0)
			{
				SendAck(cfd);
				printf("-Success\n");
				break;
			}
			else
			{
				SendFail(cfd, "this user is on line");
				printf("-The user is on line\n");
			}
		}
		else 
		{
			SendFail(cfd, "Wrong answer");
			printf("-Wrong answer\n");
		}
	}
	
	//调用更改函数
	ret = Get_Set_newpasswd(cfd, username);
	return ret;
}

int Get_Set_newpasswd(int cfd, char username[])
{
	int ret, flag;
	struct MSG msgbuf;
	char query[100], newpasswd[50];
	printf("Set newpasswd\n");

	//验证成功 获取新密码
	bzero(&msgbuf, sizeof(struct MSG));
	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret <= 0)
	{
		perror("recv CHPSWD error\n");
	}
	if(msgbuf.MSG_TYPE != MSG_CHPSWD)
	{
		printf("recv a wrong MSG when CHPSWD\n");
		return 0;
	}
	strcpy(newpasswd, msgbuf.MSG_data);
	if(strcmp(newpasswd, "0") == 0)
	{
		printf("newpasswd give up\n");
		return 0;
	}
	printf("newpasswd:%s \n", newpasswd);
	SendAck(cfd);


	//更改数据库中的密码
	sprintf(query, "update users set passwd = '%s' where username = '%s';", newpasswd, username);
	ret = mysql_real_query(mysql, query, strlen(query));
	if(ret == 0)
	{
		printf("%s CHPSWD success\n", username);
		SendAck(cfd);
		return 1;
	}
	else
	{
		printf("%s CHPSWD failed\n", username);
		SendFail(cfd, "SQL CHPSWD error");
		return 0;
	}
}

