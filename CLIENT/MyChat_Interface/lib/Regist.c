#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Send_Get.h"

extern int cfd;

void Regist()
{
	char username[50], passwd[50], question[50], answer[50];
	struct MSG msgbuf;
	int ret;
	int num, alp, i, flag;
	
	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_REGIST;
	SendMSG(msgbuf, "Login request fail\n");
	
	//吸收回车
	getchar();
	printf("请根据提示依次输入：用户名、密码、密保问题、问题答案\n");
	printf("说明：用户名需以不少于两个的字母、数字、下划线组成\n");
	while(1)
	{
		printf("请输入用户名: (输入0放弃注册)\n");
		fgets(username, sizeof(username), stdin);
		username[strlen(username)-1] = '\0';
		
		if(strcmp("0", username) == 0)
		{	
			msgbuf.MSG_TYPE = MSG_REGIST;
			strcpy(msgbuf.MSG_data, username);
			SendMSG(msgbuf, "username send fail\n");
			return;
		}
		if(strlen(username) < 2)
		{
			printf("请使用两个字符以上的用户名！\n");
			continue;
		}
		i = 0, flag = 1;
		while(username[i])
		{
			if(!isalpha(username[i]) 
				&& !(username[i] >= '0' && username[i] <= '9')
				&& username[i] != '_')
			{
				printf("请勿使用字母、数字、下划线以外的字符！\n");
				break;
			}
			i++;
		}
		if(flag == 0)
			continue;

		msgbuf.MSG_TYPE = MSG_REGIST;
		strcpy(msgbuf.MSG_data, username);
		SendMSG(msgbuf, "username send fail\n");	
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if(ret == MSG_FAIL)
		{
			//
		}
	}

	printf("说明：密码需不少于六位，且至少包含字母与数字\n");
	while(1)
	{
		printf("请输入密码: (输入0放弃注册)\n");
		fgets(passwd, sizeof(passwd), stdin);
		passwd[strlen(passwd)-1] = '\0';
		
		if(strcmp("0", passwd) == 0)
		{
			msgbuf.MSG_TYPE = MSG_REGIST;
			strcpy(msgbuf.MSG_data, passwd);
			SendMSG(msgbuf, "passwd send fail\n");
			return;
		}

		if(strlen(passwd) < 6)
		{
			printf("请使用6个字符以上的密码！\n");
			continue;
		}

		num = 0, alp = 0;
		i = 0;
		while(passwd[i])
		{
			if(passwd[i] >= '0' && passwd[i] <= '9')
				num = 1;
			if(isalpha(passwd[i]))
				alp = 1;
			i++;
		}
		if(num == 0 || alp == 0)
		{
			printf("密码需要包含数字与字母！\n");
			continue;
		}
		
		msgbuf.MSG_TYPE = MSG_REGIST;
		strcpy(msgbuf.MSG_data, passwd);
		SendMSG(msgbuf, "passwd send fail\n");
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if(ret == MSG_FAIL)
		{
			//
		}
	}

	printf("说明：密保问题需不少于5个字符\n");
	while(1)
	{
		printf("请输入密保问题: (输入0放弃注册)\n");
		fgets(question, sizeof(question), stdin);
		question[strlen(question)-1] = '\0';
		
		if(strcmp("0", question) == 0)
		{	
			msgbuf.MSG_TYPE = MSG_REGIST;
			strcpy(msgbuf.MSG_data, question);
			SendMSG(msgbuf, "question send fail\n");
			return;
		}

		if(strlen(question) < 5)
		{
			printf("请使用5个字符以上的密保问题！\n");
			continue;
		}
		
		msgbuf.MSG_TYPE = MSG_REGIST;
		strcpy(msgbuf.MSG_data, question);
		SendMSG(msgbuf, "question send fail\n");
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if(ret == MSG_FAIL)
		{
			//
		}
	}

	printf("说明：问题答案需不少于3个字符\n");
	while(1)
	{
		printf("请输入问题答案: (输入0放弃注册)\n");
		fgets(answer, sizeof(answer), stdin);
		answer[strlen(answer)-1] = '\0';
		
		if(strcmp("0", answer) == 0)
		{
			msgbuf.MSG_TYPE = MSG_REGIST;
			strcpy(msgbuf.MSG_data, answer);
			SendMSG(msgbuf, "answer send fail\n");
			return;
		}
		
		if(strlen(answer) < 3)
		{
			printf("请使用3个字符以上的问题答案！\n");
			continue;
		}
		
		msgbuf.MSG_TYPE = MSG_REGIST;
		strcpy(msgbuf.MSG_data, answer);
		SendMSG(msgbuf, "answer send fail\n");
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if(ret == MSG_FAIL)
		{
			//
		}
	}
	
	//询问注册结果
	ret = GetAck();
	if(ret == MSG_ACK)
	{
		printf("注册成功！按回车键继续\n");
		getchar();
		fflush(stdin);
	}
	else if(ret == MSG_FAIL)
	{
		printf("注册失败！按回车键继续\n");
		getchar();
		fflush(stdin);
	}
}

