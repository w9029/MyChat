#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"

//发送一次肯定信息
void SendAck(int cfd)
{
	struct MSG msgbuf;
	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_ACK;
	send(cfd, &msgbuf, sizeof(struct MSG), 0);
}

//发送一次否定信息
void SendFail(int cfd,char s[])
{
	struct MSG msgbuf;
	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_FAIL;
	strcpy(msgbuf.MSG_data, s);
	send(cfd, &msgbuf, sizeof(struct MSG), 0);
}

//发送一条自定义消息
void SendMSG(int cfd, struct MSG msgbuf)
{
	int ret;
	ret = send(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret < 0)
	{
		perror("SendMSG error\n");
	}
	else if(ret == 0)
	{
		perror("Unconnected\n");
	}
}

//接收一条自定义消息
struct MSG GetMSG(int cfd)
{
	int ret;
	struct MSG msgbuf;
	bzero(&msgbuf, sizeof(struct MSG));

	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret < 0)
	{
		perror("recv ack error\n");
		return msgbuf;
	}
	else if(ret == 0)
	{
		printf("connection is broken!\n");
		return msgbuf;
	}

	return msgbuf;
}

