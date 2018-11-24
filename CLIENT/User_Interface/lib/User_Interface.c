#include "include.h"
#include "MSG_struct.h"
#include "Net_Link.h"
#include "MSG_Recver.h"
#include "Send_Get.h"

extern int cfd;
extern char Myname[50];
int Join_flag;	//群聊标志 控制消息输出方式
int LogExit;	//控制子线程停止
int stop;		//控制多输入一个回车以刷新菜单

void Chat_to_one();
void Chat_to_all();
void Got_chat(struct MSG msgbuf);
void SENDFILE();

void User_Interface()
{
	int ret, flag = 1;
	struct MSG msgbuf;
	pthread_t tid;

	LogExit = 0;
	Join_flag = 0;
	stop = 0;

	//启动接收信息线程
	pthread_create(&tid, NULL, MSG_Recver, NULL);

	ret = GetAck();
	if(ret == MSG_ACK)
	{
		//OK
	}
	else if(ret == MSG_FAIL)
	{
		printf("User_Interface got a fail Ack\n");
		sigfun();
	}

	printf("Welcome to MyChat! %s\n", Myname);

	while(flag != 9)
	{
		fflush(stdin);
		system("clear");
		printf("当前用户: %s\n", Myname);
		printf("****** MyChat ******\n");
		printf("*                  *\n");
		printf("*   1.查看在线用户 *\n");
		printf("*   2.私聊         *\n");
		printf("*   3.群聊         *\n");
		printf("*   4.禁言用户     *\n");
		printf("*   5.解禁用户     *\n");
		printf("*   6.踢出用户     *\n");
		printf("*   7.拉回用户     *\n");
		printf("*   8.文件传输     *\n");
		printf("*   9.注销用户     *\n");
		printf("*                  *\n");
		printf("********************\n");
		printf("请输入您的选择:\n");

		scanf("%d",&flag);
		getchar();

		switch(flag)
		{
		case 1:
			//查看在线用户
			bzero(&msgbuf, sizeof(struct MSG));
			msgbuf.MSG_TYPE = MSG_CHECK_COUNT;
			SendMSG(msgbuf, "send Check Count error\n");
			getchar();
			break;
		case 2:
			//私聊
			stop = 1;
			Chat_to_one();
			//等待接收ack或fail
			while(stop)usleep(10);
			printf("请按任意键继续\n");
			getchar();
			break;
		case 3:
			//群聊
			Join_flag = 1;
			Chat_to_all();
			Join_flag = 0;
			break;
		case 4:
			//禁言
			stop = 1;

			bzero(&msgbuf, sizeof(struct MSG));
			msgbuf.MSG_TYPE = MSG_LOCK;
			//lock请求
			SendMSG(msgbuf, "send Check Count error\n");
			
			printf("请输入对方的用户名:");
			fgets(msgbuf.MSG_DES, sizeof(msgbuf.MSG_DES), stdin);
			msgbuf.MSG_DES[strlen(msgbuf.MSG_DES)-1] = '\0';
			
			SendMSG(msgbuf, "send LOCK error\n");
			
			//等待接收ack或fail
			while(stop)usleep(10);
			printf("请按任意键继续\n");
			getchar();
			break;
		case 5:
			//解禁
			stop = 1;
			bzero(&msgbuf, sizeof(struct MSG));
			msgbuf.MSG_TYPE = MSG_UNLOCK;
			//unlock请求
			SendMSG(msgbuf, "send UNLOCK error\n");
			
			printf("请输入对方的用户名:");
			fgets(msgbuf.MSG_DES, sizeof(msgbuf.MSG_DES), stdin);
			msgbuf.MSG_DES[strlen(msgbuf.MSG_DES)-1] = '\0';
			
			SendMSG(msgbuf, "send Check Count error\n");
			
			//等待接收ack或fail
			while(stop)usleep(10);
			printf("请按任意键继续\n");
			getchar();
			break;
		case 6:
			//踢人
			stop = 1;
			bzero(&msgbuf, sizeof(struct MSG));
			msgbuf.MSG_TYPE = MSG_KICK;
			//kick请求
			SendMSG(msgbuf, "send KICK error\n");
			
			printf("请输入对方的用户名:");
			fgets(msgbuf.MSG_DES, sizeof(msgbuf.MSG_DES), stdin);
			msgbuf.MSG_DES[strlen(msgbuf.MSG_DES)-1] = '\0';
			
			SendMSG(msgbuf, "send Check Count error\n");
			
			//等待接收ack或fail
			while(stop)usleep(10);
			printf("请按任意键继续\n");
			getchar();
			break;
		case 7:
			//拉回
			stop = 1;
			bzero(&msgbuf, sizeof(struct MSG));
			msgbuf.MSG_TYPE = MSG_UNKICK;
			//unlock请求
			SendMSG(msgbuf, "send UNKICK error\n");
			
			printf("请输入对方的用户名:");
			fgets(msgbuf.MSG_DES, sizeof(msgbuf.MSG_DES), stdin);
			msgbuf.MSG_DES[strlen(msgbuf.MSG_DES)-1] = '\0';
			
			SendMSG(msgbuf, "send Check Count error\n");
			
			//等待接收ack或fail
			while(stop)usleep(10);
			printf("请按任意键继续\n");
			getchar();
			break;
		case 8:
			//文件传输
			SENDFILE();
			break;
		case 9:
			//logout
			LogExit = 1;
			Logout();
			break;
		default:
			break;
		}//switch
		
	}
}

void *MSG_Recver()
{
	int ret;
	struct MSG msgbuf;

	while(LogExit == 0)
	{
		msgbuf = GetMSG();
		//puts(msgbuf.MSG_data);
		switch(msgbuf.MSG_TYPE)
		{
		case MSG_ACK:
			//收到ACK 结束等待
			stop = 0;
			break;
		case MSG_CHECK_COUNT:
			//收到了在线用户查询的结果
			printf("%s\n", msgbuf.MSG_data);
			printf("按任意键继续\n");
			break;
		case MSG_CHAT_Private:
			//收到了私聊chat消息
			Got_chat(msgbuf);
			break;
		case MSG_CHAT_ALL:
			//收到了群聊chat消息
			Got_chat(msgbuf);
			break;
		case MSG_FAIL:
			//收到了fail消息 结束等待
			printf("%s\n", msgbuf.MSG_data);
			stop = 0;
			break;
		default:
			break;
		}
	}
}

//收到聊天消息
void Got_chat(struct MSG msgbuf)
{
	int ret;

	//收到私聊消息
	if(msgbuf.MSG_TYPE == MSG_CHAT_Private)
	{
		printf("<收到消息> %s(私聊): %s\n", msgbuf.MSG_SRC, msgbuf.MSG_data);
	}
	else if(msgbuf.MSG_TYPE == MSG_CHAT_ALL)
	{
		if(Join_flag == 0)
			printf("<收到消息> %s(群聊): %s\n", msgbuf.MSG_SRC, msgbuf.MSG_data);
		else
		{
			printf("%s: %s\n", msgbuf.MSG_SRC, msgbuf.MSG_data);
		}
	}
	else
	{
		printf("Got chat MSG_TYPE error\n");
	}

}

//发送私聊消息
void Chat_to_one()
{
	int ret;
	struct MSG msgbuf;

	//发送聊天请求
	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_CHAT;
	SendMSG(msgbuf, "send Check Count error\n");

	//构造消息并发送
	bzero(&msgbuf, sizeof(struct MSG));
	strcpy(msgbuf.MSG_SRC, Myname);

	printf("请输入对方的用户名:");
	fgets(msgbuf.MSG_DES, sizeof(msgbuf.MSG_DES), stdin);
	msgbuf.MSG_DES[strlen(msgbuf.MSG_DES)-1] = '\0';

	printf("请输入发送内容:");
	fgets(msgbuf.MSG_data, sizeof(msgbuf.MSG_data), stdin);
	msgbuf.MSG_data[strlen(msgbuf.MSG_data)-1] = '\0';

	msgbuf.MSG_TYPE = MSG_CHAT_Private;
	SendMSG(msgbuf, "send Private Chat error\n");
}

void Chat_to_all()
{
	int ret;
	struct MSG msgbuf;
	char MSG_word[1000];
	
	printf("------ 已进入聊天室 输入 “EXIT” 以退出 ------\n");
	while(1)
	{
		//获取输入
		fgets(MSG_word, sizeof(MSG_word), stdin);
		MSG_word[strlen(MSG_word)-1] = '\0';
		
		if(strcmp(MSG_word, "EXIT") == 0)
			break;
		
		//发送聊天请求
		bzero(&msgbuf, sizeof(struct MSG));
		msgbuf.MSG_TYPE = MSG_CHAT;
		SendMSG(msgbuf, "send Check Count error\n");
		
		//构造消息
		bzero(&msgbuf, sizeof(struct MSG));
		strcpy(msgbuf.MSG_SRC, Myname);		//源用户名
		msgbuf.MSG_TYPE = MSG_CHAT_ALL;		//消息类型
		strcpy(msgbuf.MSG_data, MSG_word);	//消息内容
		
		//发送
		SendMSG(msgbuf, "send Private Chat error\n");
	}
}

void SENDFILE()
{
	
}

