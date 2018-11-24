#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern struct Linker Linker_head;	//接入者链表
extern struct Linker *Last_Linker;	//指向最后一个接入者
extern int sfd;
extern int ret;

void *Service(void *arg)
{
	struct Linker *this_Linker;
	struct MSG msgbuf;
	int cfd, ret;
	
	cfd = *(int*)arg;

	//发送一次ack应答信息
	SendAck(cfd);

	while(1)
	{
		bzero(&msgbuf, sizeof(struct MSG));
		ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
		if(ret > 0)
		{
			switch(msgbuf.MSG_TYPE)
			{
				case MSG_LOGIN:
					//登陆
					Ser_Login(cfd);
					break;
				case MSG_REGIST:
					//注册

					break;
				case MSG_CHPSWD:
					//更改密码

					break;
				case MSG_CHECK_COUNT:
					//查看在线人数

					break;
				case MSG_CHAT:
					//聊天

					break;
				case MSG_LOCK:
					//禁言

					break;
				case MSG_UNLOCK:
					//解禁

					break;
				case MSG_KICK:
					//踢出

					break;
				case MSG_FILE_NAME:
					//文件传输

					break;
				case MSG_TEMP:
					//其他数据

					break;
				default:
					break;
			}
		}
	}//while

}


