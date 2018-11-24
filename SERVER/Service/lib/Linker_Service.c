#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Cli_Exit.h"

extern struct Linker Linker_head;	//接入者链表
extern struct Linker *Last_Linker;	//指向最后一个接入者
extern int sfd;
extern int ret;

void *Linker_Service(void *arg)
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
					Ser_Regist(cfd);
					break;
				case MSG_CHPSWD:
					//更改密码
					Ser_CHPSWD(cfd);
					break;
				case MSG_EXIT:
					//注销
					Ser_cliExit(cfd);
					break;
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


