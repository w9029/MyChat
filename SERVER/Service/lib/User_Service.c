#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Cli_Exit.h"
#include "Get_user_stat.h"
#include "Get_user_cfd.h"

void CheckCount(int cfd);
void CHAT(int cfd);
void LOCK(int cfd);
void UNLOCK(int cfd);
void KICK(int cfd);
void UNKICK(int cfd);
void FILEACK(int cfd);
void SENDFILEMSG(int cfd, struct MSG msgbuf);

extern struct Linker Linker_head;	//接入者链表
extern struct Linker *Last_Linker;	//指向最后一个接入者
extern int sfd;
extern int ret;

void User_Service(int cfd, char username[])
{
	struct Linker *this_Linker;
	struct MSG msgbuf;
	int ret;

	printf("%s entered User_Service\n", username);

	//发送一次ack应答信息
	SendAck(cfd);
	printf("User Service send first ack ok\n");

	while(1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret > 0)
		{
			switch(msgbuf.MSG_TYPE)
			{
			case MSG_CHECK_COUNT:
				//查询在线人数
				CheckCount(cfd);
				break;
			case MSG_CHAT:
				//聊天
				printf("got MSG_CHAT\n");
				CHAT(cfd);
				break;
			case MSG_LOCK:
				//禁言
				LOCK(cfd);
				break;
			case MSG_UNLOCK:
				//解禁
				UNLOCK(cfd);
				break;
			case MSG_KICK:
				//踢人
				KICK(cfd);
				break;
			case MSG_UNKICK:
				//解禁
				UNKICK(cfd);
				break;
			case MSG_FILE_NAME:
				//FILE
				SENDFILEMSG(cfd, msgbuf);
				break;
			case MSG_FILE_ACK:
				//FILEACK
				SENDFILEMSG(cfd, msgbuf);
				break;
			case MSG_FILE_DATA:
				//FILE_DATA
				SENDFILEMSG(cfd, msgbuf);
				break;
			case MSG_FILE_DONE:
				SENDFILEMSG(cfd, msgbuf);
				break;
			case MSG_FILE_ERROR:
				//FILE_ERROR
				SENDFILEMSG(cfd, msgbuf);
				break;
			case MSG_EXIT:
				//注销
				Ser_cliExit(cfd);
				return;
			default:
				break;
			}
		}
		else if(ret == 0)
		{
			printf("Unconnected\n");
			break;
		}
	}//while
}

void CheckCount(int cfd)
{
	int ret, count = 0;
	struct Linker *temp;
	struct MSG msgbuf;
	char restemp1[1000] = "", 
		 restemp2[1000] = "", 
		 resault[1000] = "当前在线人数:";

	printf("Check Users\n");

	temp = Linker_head.next;
	while(temp)
	{
		strcat(restemp1, temp->Linker_username);
		if(temp == Linker_head.next)
			strcat(restemp1, "(群主)");
		strcat(restemp1, " ");
		temp = temp->next;
		count++;
	}
	sprintf(restemp2, "当前在线人数:%d\n用户名: ", count);
	sprintf(resault, "%s%s", restemp2, restemp1);

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_CHECK_COUNT;
	strcpy(msgbuf.MSG_data, resault);
	SendMSG(cfd, msgbuf);
}

void CHAT(int cfd)
{
	int ret;
	struct Linker *temp;
	struct MSG msgbuf;

	//接收消息
	msgbuf = GetMSG(cfd);

	//判断发送源状态
	ret = Get_user_stat(msgbuf.MSG_SRC);
	if(ret == 0)
	{
		printf("chat SRC is not online\n");
		return;
	}
	else if(ret == Linker_LOCK)
	{
		printf("private send fail: src lock\n");
		SendFail(cfd, "发送失败！您正处于禁言状态");
		return;
	}
	else if(ret == Linker_KICK)
	{
		printf("private send fail: src kick\n");
		SendFail(cfd, "发送失败！您正处于被踢出的状态");
		return;
	}
	else if(ret == Linker_FREE)
	{
		//源地址用户free 判断私聊或群聊消息
		if(msgbuf.MSG_TYPE == MSG_CHAT_Private)
		{
			//私聊消息
			printf("Chat Private\n");

			ret = Get_user_stat(msgbuf.MSG_DES);
			if(ret == 0)
			{
				printf("private send fail: des not online\n");
				SendFail(cfd, "发送失败！对方不在线");
				return;
			}
			else if(ret == Linker_KICK)
			{
				printf("private send fail: des kick");
				SendFail(cfd, "发送失败！对方正处于被踢出的状态");
				return;
			}
			else if(ret == Linker_FREE || ret == Linker_LOCK)
			{
				int descfd;
				descfd = Get_user_cfd(msgbuf.MSG_DES);
				if(descfd == -1)
				{
					printf("private send fail: des not online");
					SendFail(cfd, "发送失败！服务器无法获取目标cfd");
					return;
				}
				//成功获取descfd 发送给目标 并响应源
				SendMSG(descfd, msgbuf);
				SendAck(cfd);
				return;
			}
		}
		else if(msgbuf.MSG_TYPE == MSG_CHAT_ALL)
		{
			//群聊消息
			printf("Chat ALL\n");
			struct Linker *temp;
			temp = Linker_head.next;
			while(temp)
			{
				if(strcmp(temp->Linker_username, msgbuf.MSG_SRC) != 0
						&& temp->Linker_stat != Linker_KICK)
				{
					int descfd;
					descfd = Get_user_cfd(temp->Linker_username);
					if(descfd == -1)
					{
						printf("send fail: des not online");
					}
					//成功获取descfd 发送给目标 并响应源
					SendMSG(descfd, msgbuf);
				}
				temp = temp->next;
			}
		}
		else
		{
			printf("Wrong MSG when chat\n");
			SendFail(cfd, "失败！对方网络繁忙\n");
		}
	}
}

void LOCK(int cfd)
{
	int ret, flag;
	struct Linker *temp;
	struct MSG msgbuf;
	char DesUser[50];

	//接收消息
	msgbuf = GetMSG(cfd);
	strcpy(DesUser, msgbuf.MSG_DES);

	//检查Linker链表
	if(Linker_head.next == NULL)
	{
		printf("Lock fail, Linker_list is NULL\n");
		SendFail(cfd, "Linker_list NULL error");
		return;
	}

	//验证权限
	if(cfd != Linker_head.next->Linker_cfd)
	{
		printf("Lock fail, SRC do not have power\n");
		SendFail(cfd, "Sorry, you don't have power");
		return;
	}

	//通过权限验证 寻找lock目标
	temp = Linker_head.next;
	while(temp)
	{
		if(strcmp(temp->Linker_username, DesUser) == 0)
		{
			//找到目标 判断目标当前状态
			if(temp->Linker_stat == Linker_FREE)
			{
				//禁言该用户
				temp->Linker_stat = Linker_LOCK;

				//利用SendFail提示成功
				SendFail(cfd, "Lock success");
				SendFail(temp->Linker_cfd, "您被禁言了！");
				return;
			}
			else if(temp->Linker_stat == Linker_LOCK)
			{
				bzero(&msgbuf, sizeof(struct MSG));
				SendFail(cfd, "对方已被禁言！不可重复操作");
				return;
			}
			else if(temp->Linker_stat == Linker_KICK)
			{
				bzero(&msgbuf, sizeof(struct MSG));
				SendFail(cfd, "对方已被踢出！无法禁言");
				return;
			}
		}
		temp = temp->next;
	}
	//未找到目标
	SendFail(cfd, "禁言失败！未登录的用户");
}

void UNLOCK(int cfd)
{
	int ret, flag;
	struct Linker *temp;
	struct MSG msgbuf;
	char DesUser[50];

	//接收消息
	msgbuf = GetMSG(cfd);
	strcpy(DesUser, msgbuf.MSG_DES);

	//检查Linker链表
	if(Linker_head.next == NULL)
	{
		printf("Unlock fail, Linker_list is NULL\n");
		SendFail(cfd, "Linker_list NULL error");
		return;
	}

	//验证权限
	if(cfd != Linker_head.next->Linker_cfd)
	{
		printf("Unlock fail, SRC do not have power\n");
		SendFail(cfd, "Sorry, you don't have power");
		return;
	}

	//通过权限验证 寻找unlock目标
	temp = Linker_head.next;
	while(temp)
	{
		if(strcmp(temp->Linker_username, DesUser) == 0)
		{
			//找到目标 判断目标当前状态
			if(temp->Linker_stat == Linker_LOCK)
			{
				//解禁该用户
				temp->Linker_stat = Linker_FREE;

				//利用SendFail提示成功
				SendFail(cfd, "Unlock success");
				SendFail(temp->Linker_cfd, "您被解除了禁言！");
				return;
			}
			else if(temp->Linker_stat == Linker_FREE)
			{
				bzero(&msgbuf, sizeof(struct MSG));
				SendFail(cfd, "对方未被禁言！");
				return;
			}
			else if(temp->Linker_stat == Linker_KICK)
			{
				bzero(&msgbuf, sizeof(struct MSG));
				SendFail(cfd, "对方已被踢出！");
				return;
			}
		}
		temp = temp->next;
	}
	//未找到目标
	SendFail(cfd, "解禁失败！未登录的用户");
}

void KICK(int cfd)
{
	int ret, flag;
	struct Linker *temp;
	struct MSG msgbuf;
	char DesUser[50];

	//接收消息
	msgbuf = GetMSG(cfd);
	strcpy(DesUser, msgbuf.MSG_DES);

	//检查Linker链表
	if(Linker_head.next == NULL)
	{
		printf("Kick fail, Linker_list is NULL\n");
		SendFail(cfd, "Linker_list NULL error");
		return;
	}

	//验证权限
	if(cfd != Linker_head.next->Linker_cfd)
	{
		printf("Kick fail, SRC do not have power\n");
		SendFail(cfd, "Sorry, you don't have power");
		return;
	}

	//通过权限验证 寻找kick目标
	temp = Linker_head.next;
	while(temp)
	{
		if(strcmp(temp->Linker_username, DesUser) == 0)
		{
			//找到目标 判断目标当前状态
			if(temp->Linker_stat != Linker_KICK)
			{
				//踢出该用户
				temp->Linker_stat = Linker_KICK;

				//利用SendFail提示成功
				SendFail(cfd, "Kick success");
				SendFail(temp->Linker_cfd, "您被踢出了群聊！");
				return;
			}
			else if(temp->Linker_stat == Linker_KICK)
			{
				bzero(&msgbuf, sizeof(struct MSG));
				SendFail(cfd, "对方已被踢出！无法重复操作");
				return;
			}
		}
		temp = temp->next;
	}
	//未找到目标
	SendFail(cfd, "踢人失败！未登录的用户");
}

void UNKICK(int cfd)
{
	int ret, flag;
	struct Linker *temp;
	struct MSG msgbuf;
	char DesUser[50];

	//接收消息
	msgbuf = GetMSG(cfd);
	strcpy(DesUser, msgbuf.MSG_DES);

	//检查Linker链表
	if(Linker_head.next == NULL)
	{
		printf("Unkick fail, Linker_list is NULL\n");
		SendFail(cfd, "Linker_list NULL error");
		return;
	}

	//验证权限
	if(cfd != Linker_head.next->Linker_cfd)
	{
		printf("Unkick fail, SRC do not have power\n");
		SendFail(cfd, "Sorry, you don't have power");
		return;
	}

	//通过权限验证 寻找kick目标
	temp = Linker_head.next;
	while(temp)
	{
		if(strcmp(temp->Linker_username, DesUser) == 0)
		{
			//找到目标 判断目标当前状态
			if(temp->Linker_stat == Linker_KICK)
			{
				//拉回该用户
				temp->Linker_stat = Linker_KICK;

				//利用SendFail提示成功
				SendFail(cfd, "Unkick success");
				SendFail(temp->Linker_cfd, "您拉回了群聊！");
				return;
			}
			else if(temp->Linker_stat != Linker_KICK)
			{
				bzero(&msgbuf, sizeof(struct MSG));
				SendFail(cfd, "对方未被踢出！无法拉回");
				return;
			}
		}
		temp = temp->next;
	}
	//未找到目标
	SendFail(cfd, "拉回失败！未登录的用户");
}

void SENDFILEMSG(int cfd, struct MSG msgbuf)
{
	char DesUser[50];
	int descfd;

	//获取对方cfd
	strcpy(DesUser, msgbuf.MSG_DES);
	descfd = Get_user_cfd(DesUser);
	
	//获取失败
	if(descfd == -1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		msgbuf.MSG_TYPE = MSG_FILE_ERROR;
		printf("SendFILE get descfd fail\n");
		strcpy(msgbuf.MSG_data, "发送失败！对方不在线");
		SendMSG(cfd, msgbuf);
		return;
	}
	
	//获取成功
	SendMSG(descfd, msgbuf);
}





