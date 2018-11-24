#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern struct Linker Linker_head;	//在线人链表头
extern struct Linker *Last_Linker;	//指向最后一个在线人
extern int sfd;
extern MYSQL *mysql;

void Ser_Login(int cfd)
{
	int ret;
	struct MSG msgbuf;
	struct Linker *temp;
	char username[50], passwd[50], sql_passwd[50];
	char query[100];
	printf("Login\n");
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
		if(msgbuf.MSG_TYPE != MSG_LOGIN)
		{
			printf("recv a wrong MSG when Login\n");
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
			perror("recv Login error\n");
			break;
		}
		if(msgbuf.MSG_TYPE != MSG_LOGIN)
		{
			printf("recv a wrong MSG when Login\n");
			return;
		}
		strcpy(passwd, msgbuf.MSG_data);
		if(strcmp(passwd, "0") == 0)
		{
			printf("passwd give up\n");
			return;
		}

		printf("passwd:%s ", passwd);
		if(strcmp(passwd, sql_passwd) == 0)
		{
			//密码正确 判断用户是否在线
			struct Linker *temp;
			int flag = 0;
			temp = Linker_head.next;
			while(temp)
			{
				if(strcmp(username, temp->Linker_username) == 0)
				{
					flag = 1;
				}
				temp = temp->next;
			}
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
	//登陆成功
	temp = (struct Linker*)malloc(sizeof(struct Linker));
	strcpy(temp->Linker_username, username);
	temp->Linker_cfd = cfd;
	temp->Linker_stat = Linker_FREE;
	
	temp->next = NULL;
	Last_Linker->next = temp;
	Last_Linker = temp;
	printf("%s Login success\n", temp->Linker_username);
	User_Service(cfd, username);
}


