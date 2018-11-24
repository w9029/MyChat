#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Send_Get.h"

extern int cfd;
extern char Myname[50];

void Login()
{
	char username[50];
	char passwd[50];
	struct MSG msgbuf;
	int ret;

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_LOGIN;
	SendMSG(msgbuf, "Login request fail\n");
	
	//吸收回车
	getchar();
	while(1)
	{
		printf("请输入用户名: ");
		fgets(username, sizeof(username), stdin);
		//gets(username);
		//if(strlen(username) < 1)
		//	continue;
		username[strlen(username)-1] = '\0';
		msgbuf.MSG_TYPE = MSG_LOGIN;
		strcpy(msgbuf.MSG_data, username);
		
		SendMSG(msgbuf, "username send fail\n");
		if(strcmp("0", username) == 0)
			return;
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if(ret == MSG_FAIL)
		{
			printf("input 0 to exit\n");
		}
	}

	while(1)
	{
		printf("请输入密码: ");
		//fflush(stdin);
		//gets(passwd);
		fgets(passwd, sizeof(passwd), stdin);
		passwd[strlen(passwd)-1] = '\0';
		msgbuf.MSG_TYPE = MSG_LOGIN;
		strcpy(msgbuf.MSG_data, passwd);
		SendMSG(msgbuf, "passwd send fail\n");
		if(strcmp("0", passwd) == 0)
			return;
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if(ret == MSG_FAIL)
		{
			printf("input 0 to exit\n");
		}
	}
	
	//登陆成功
	strcpy(Myname, username);
	printf("登陆成功！按回车键继续\n");
	getchar();
	fflush(stdin);

	User_Interface();	
}

