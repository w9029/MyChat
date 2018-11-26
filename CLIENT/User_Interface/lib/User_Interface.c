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
int FileAck;	//控制等待fileAck
int RecvingFile;//控制主函数停止接收键盘输入

void Chat_to_one();
void Chat_to_all();
void Got_chat(struct MSG msgbuf);
void RECVFILE(struct MSG msgbuf);
void SENDFILE();

void User_Interface()
{
	int ret, flag = 1;
	struct MSG msgbuf;
	pthread_t tid;

	LogExit = 0;
	Join_flag = 0;
	stop = 0;
	FileAck = 0;
	RecvingFile = 0;

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
		
		//当前正在接收文件名 把键盘输入权交给文件接收函数
		int k = 0;
		while(RecvingFile == 1)
		{
			k = 1;
			usleep(10);
		}
		//放弃本次输入
		if(k == 1)
		{
			k = 0;
			continue;
		}
		
		if(RecvingFile == 1)
			continue;

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
		case MSG_FILE_NAME:
			//收到了文件名
			RECVFILE(msgbuf);
			break;
		case MSG_FILE_ACK:
			//收到了文件ACK
			FileAck = MSG_FILE_ACK;
			break;
		case MSG_FILE_DATA:
			//收到了文件内容
			RECVFILE(msgbuf);
			break;
		case MSG_FILE_DONE:
			//收到了文件完成消息
			FileAck = MSG_FILE_DONE;
			RECVFILE(msgbuf);
			break;
		case MSG_FILE_ERROR:
			//收到了文件失败消息
			printf("%s\n", msgbuf.MSG_data);
			FileAck = MSG_FILE_ERROR;
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
	int ret;
	struct MSG msgbuf;
	char username[50], filename[50];
	FILE *fp;
	
	//构造消息 发送文件名消息
	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_FILE_NAME;		//类型
	strcpy(msgbuf.MSG_SRC, Myname);			//源

	printf("请输入对方的用户名:\n");
	fgets(username, sizeof(username), stdin);
	username[strlen(username)-1] = '\0';
	strcpy(msgbuf.MSG_DES, username);		//目的

	printf("请输入文件名:\n");
	fgets(filename, sizeof(filename), stdin);
	filename[strlen(filename)-1] = '\0';	//文件名
	strcpy(msgbuf.MSG_FILENAME, filename);

	//判断文件是否存在并可读
	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("打开失败！文件不存在或不可读！\n");
		printf("按回车键继续\n");
		getchar();
		return;
	}

	//发送
	FileAck = 0;	//避免特殊情况
	SendMSG(msgbuf, "send file name error\n");
	printf("等待对方同意接受该文件...\n");
	
	//防止用户多输入
	fflush(stdin);

	while(FileAck == 0)
	{
		usleep(10);
	}

	//收到ack 开始传输文件
	if(FileAck == MSG_FILE_ACK)
	{
		printf("对方已同意传输文件！\n");
		printf("正在传输文件...\n");
		//用于等待ack
		FileAck = 0;
		
		//开始传输
		while(1)
		{
			//构造消息结构体
			bzero(&msgbuf, sizeof(struct MSG));
			strcpy(msgbuf.MSG_SRC, Myname);
			strcpy(msgbuf.MSG_DES, username);
			strcpy(msgbuf.MSG_FILENAME, filename);
			
			//读取文件
			ret = fread(msgbuf.MSG_data, sizeof(char), 1000, fp);
			//读取失败
			if(ret <= 0)
			{
				if(feof(fp))	//文件结束
				{
					printf("文件传输结束！\n");
					msgbuf.MSG_TYPE = MSG_FILE_DONE;
					FileAck = 0;	//避免特殊情况
					SendMSG(msgbuf, "send file error\n");
				}
				if(ferror(fp))	//错误 文件中止
				{
					printf("文件传输结束！\n");
					msgbuf.MSG_TYPE = MSG_FILE_ERROR;
					strcpy(msgbuf.MSG_data, "file error\n");
					FileAck = 0;	//避免特殊情况
					SendMSG(msgbuf, "send file error\n");
				}
				fclose(fp);		//关闭文件
				break;
			}
			//读取成功 
			msgbuf.MSG_TYPE = MSG_FILE_DATA;
			//发送
			FileAck = 0;	//避免特殊情况
			SendMSG(msgbuf, "send file error\n");
			
			//测试
			//printf("send file data ok,waiting ack\n");

			//等待回应
			while(FileAck == 0)
			{
				usleep(10);
			}

			//测试
			//printf("got ack\n");
			
			//判断收到的信号
			if(FileAck == MSG_FILE_ERROR)
			{
				FileAck = 0;
				fclose(fp);
				break;
			}
			else if(FileAck != MSG_FILE_ACK)
			{
				printf("Wrong MSG when got FileAck/FileERROR\n");
				FileAck = 0;
				fclose(fp);
				break;
			}
		}
	}
	//文件接收出错或对方不同意接收
	else if(FileAck == MSG_FILE_ERROR)
	{
		FileAck = 0;
		fclose(fp);
	}

	printf("按任意键继续\n");
	getchar();
}

void RECVFILE(struct MSG msgbuf)
{
	char reply[50] = "", temp[50];
	char localfilename[256];
	//说明：此处DESname为发送源的名字，即相对接收者而言的DES
	char DESname[50], FILEname[50];	
	FILE *fp;

	strcpy(DESname, msgbuf.MSG_SRC);
	strcpy(FILEname, msgbuf.MSG_FILENAME);

	//构造文件名
	sprintf(localfilename, "./GotFile/Got_%s_%s", 
			DESname, FILEname);
	
	//构造回应消息：交换源名和目的名
	strcpy(temp, msgbuf.MSG_SRC);
	strcpy(msgbuf.MSG_SRC, msgbuf.MSG_DES);
	strcpy(msgbuf.MSG_DES, temp);
	
	if(msgbuf.MSG_TYPE == MSG_FILE_NAME)
	{
		printf("%s向你发送了一个名为%s的文件，是否接收？(Yes/No)\n", 
				DESname, FILEname);
		
		//阻塞主线程的键盘接收
		RecvingFile = 1;
		
		//停止主线程的阻塞
		fflush(stdin);
		sprintf(stdin, "1\n");
		
		while(1)
		{
			fflush(stdin);
			fgets(reply, sizeof(reply), stdin);
			reply[strlen(reply)-1] = '\0';
			if(strcmp("Yes", reply) == 0)
			{
				//停止主线程输入阻塞
				RecvingFile = 0;
				//构造MSG_FILE_ACK消息 发回给对方
				msgbuf.MSG_TYPE = MSG_FILE_ACK;
				//发送
				SendMSG(msgbuf, "send fileAck fail\n");
				//仅需退出此while
				printf("正在传输文件...\n");
				break;
			}
			else if(strcmp("No", reply) == 0)
			{
				printf("您拒绝接收了该文件\n");
				//构造MSG_FILE_ERROR消息 发回给对方
				msgbuf.MSG_TYPE = MSG_FILE_ERROR;
				strcpy(msgbuf.MSG_data, "对方拒绝接收！");
				//发送
				SendMSG(msgbuf, "send fileError fail\n");
				
				printf("按回车键继续\n");
				getchar();
				
				//停止主线程输入阻塞
				RecvingFile = 0;
				//退出该函数
				return;
			}
			else
			{
				printf("请输入Yes或No来进行回应！\n");
			}
		}
		fp = fopen(localfilename, "w");
		if(fp == NULL)
		{
			printf("文件打开失败\n");
			//构造MSG_FILE_ERROR消息 发回给对方
			msgbuf.MSG_TYPE = MSG_FILE_ERROR;
			strcpy(msgbuf.MSG_data, "对方打开文件失败");
			//发送
			SendMSG(msgbuf, "send file_open_error fail\n");
			
			printf("按回车键继续\n");
			return;
		}
		else
		{
			//文件正常创建或打开
			msgbuf.MSG_TYPE = MSG_FILE_ACK;
			SendMSG(msgbuf, "send file_Ack fail\n");
			fclose(fp);
			return;
		}
	}
	else if(msgbuf.MSG_TYPE == MSG_FILE_DATA)
	{
		fp = fopen(localfilename, "a");
		if(fp == NULL)
		{
			printf("文件打开失败\n");
			//构造MSG_FILE_ERROR消息 发回给对方
			msgbuf.MSG_TYPE = MSG_FILE_ERROR;
			strcpy(msgbuf.MSG_data, "对方打开文件失败");
			//发送
			SendMSG(msgbuf, "send file_open_error fail\n");
			return;
		}
		else
		{
			//正常打开 进行追加
			fprintf(fp, msgbuf.MSG_data, strlen(msgbuf.MSG_data));
			fclose(fp);
			
			//发送fileAck
			msgbuf.MSG_TYPE = MSG_FILE_ACK;
			//发送
			SendMSG(msgbuf, "send file_open_error fail\n");
			return;
		}
		
	}
	else if(msgbuf.MSG_TYPE == MSG_FILE_DONE)
	{

		RecvingFile = 1;
		printf("文件传输成功！路径为%s\n", localfilename);
		sleep(1);
		RecvingFile = 0;
	}
	else if(msgbuf.MSG_TYPE == MSG_FILE_ERROR)
	{
		printf("文件传输中止！");
	}
}




