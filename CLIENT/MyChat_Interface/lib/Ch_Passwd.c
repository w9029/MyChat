#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Send_Get.h"

extern int cfd;

int Ch_by_PW(char username[]);
int Ch_by_Q(char username[]);

void Ch_Passwd()
{
	int ret, flag, choice;
	struct MSG msgbuf;
	char username[50];

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_CHPSWD;
	SendMSG(msgbuf, "CHPSWD request fail\n");

	//输入用户名
	getchar();
	while(1)
	{
		printf("请输入用户名: ");
		fgets(username, sizeof(username), stdin);
		username[strlen(username)-1] = '\0';
		msgbuf.MSG_TYPE = MSG_CHPSWD;
		strcpy(msgbuf.MSG_data, username);
		SendMSG(msgbuf, "username send fail\n");
		if(strcmp("0", username) == 0)
		{
			printf("放弃更改密码 按回车键继续\n");
			getchar();
			fflush(stdin);
			return;
		}
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

	//选择更改方式
	flag = 0;
	choice = 0;
	while(choice != 3 && flag != 1)
	{
		system("clear");
		fflush(stdin);
		printf("1.使用原密码进行更改\n");
		printf("2.忘记密码，验证密保问题来更改\n");
		printf("3.放弃本次操作\n");
		printf("请输入您的选择:");

		scanf("%d", &choice);

		switch(choice)
		{
		case 1:
			//用原密码更改
			flag = Ch_by_PW(username);
			break;
		case 2:
			//验证密保问题
			flag = Ch_by_Q(username);
			break;
		case 3:
			printf("exit CHPSWD\n");
			msgbuf.MSG_TYPE = MSG_CHPSWD;
			strcpy(msgbuf.MSG_data, "give up CHPSWD");
			SendMSG(msgbuf, "CHPSWD give up send fail\n");
			break;
		default:
			printf("unknown choice, please try again\n");
			break;
		}
	}
}

int Ch_by_PW(char username[])
{
	char passwd[50], newpasswd[50];
	struct MSG msgbuf;
	int ret;

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_CHPSWD;
	strcpy(msgbuf.MSG_data, "Ch_by_PW");
	SendMSG(msgbuf, "CHPSWD request fail\n");

	getchar();
	while(1)
	{
		printf("请输入原密码: (输入0放弃本次操作)\n");
		fflush(stdin);
		//gets(passwd);
		fgets(passwd, sizeof(passwd), stdin);
		if(strlen(passwd) <= 1)
			continue;
		passwd[strlen(passwd)-1] = '\0';
		msgbuf.MSG_TYPE = MSG_CHPSWD;
		strcpy(msgbuf.MSG_data, passwd);
		SendMSG(msgbuf, "passwd send fail\n");
		if(strcmp("0", passwd) == 0)
			return 0;

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

	//验证成功
	return Ch_new(username);
}

int Ch_by_Q(char username[])
{
	char question[50], answer[50];
	struct MSG msgbuf;
	int ret;

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_CHPSWD;
	strcpy(msgbuf.MSG_data, "Ch_by_Q");
	SendMSG(msgbuf, "CHPSWD request fail\n");
	
	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret <= 0)
	{
		perror("Question recv error\n");
		return 0;
	}
	if(msgbuf.MSG_TYPE != MSG_CHPSWD)
	{
		printf("MSG is not Question\n");
		return 0;
	}
	strcpy(question, msgbuf.MSG_data);
	
	getchar();
	while(1)
	{
		printf("问题：%s\n", question);
		printf("请输入答案:(输入0放弃本次操作)\n");
		fflush(stdin);
		fgets(answer, sizeof(answer), stdin);
		if(strlen(answer) <= 1)
			continue;
		answer[strlen(answer)-1] = '\0';
		msgbuf.MSG_TYPE = MSG_CHPSWD;
		strcpy(msgbuf.MSG_data, answer);
		SendMSG(msgbuf, "passwd send fail\n");
		if(strcmp("0", answer) == 0)
			return 0;

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

	//验证成功
	return Ch_new(username);
}

int Ch_new(char username[])
{
	char newpasswd[50];
	struct MSG msgbuf;
	int ret, i , alp, num;
	
	printf("验证成功！\n");
	while(1)
	{
		printf("请输入新密码：(输入0放弃本次操作)\n");
		fflush(stdin);
		//gets(passwd);
		fgets(newpasswd, sizeof(newpasswd), stdin);
		if(strlen(newpasswd) <= 1)
			continue;
		newpasswd[strlen(newpasswd)-1] = '\0';

		if(strcmp("0", newpasswd) == 0)
		{
			msgbuf.MSG_TYPE = MSG_REGIST;
			strcpy(msgbuf.MSG_data, newpasswd);
			SendMSG(msgbuf, "passwd send fail\n");
			return 0;
		}

		//判断密码合法性
		if(strlen(newpasswd) < 6)
		{
			printf("请使用6个字符以上的密码！\n");
			continue;
		}

		num = 0;
		alp = 0;
		i = 0;
		while(newpasswd[i])
		{
			if(newpasswd[i] >= '0' && newpasswd[i] <= '9')
				num = 1;
			if(isalpha(newpasswd[i]))
				alp = 1;
			i++;
		}
		if(num == 0 || alp == 0)
		{
			printf("密码需要包含数字与字母！\n");
			continue;
		}
		
		//通过合法性检测，发送新密码
		msgbuf.MSG_TYPE = MSG_CHPSWD;
		strcpy(msgbuf.MSG_data, newpasswd);
		SendMSG(msgbuf, "newpasswd send fail\n");
		
		ret = GetAck();
		if(ret == MSG_ACK)
		{
			break;
		}
		else if (ret == MSG_FAIL)
		{
			//
		}
	}
	
	//得到数据库处理结果
	ret = GetAck();
	if(ret == MSG_ACK)
	{
		printf("修改成功!按回车键继续\n");
		getchar();
		return 1;
	}
	else if (ret == MSG_FAIL)
	{
		//
		printf("修改失败!按回车键继续\n");
		getchar();
		return 0;
	}
	else
	{
		printf("wrong MSG when CHPSWD\n");
		return 0;
	}
	
}	
