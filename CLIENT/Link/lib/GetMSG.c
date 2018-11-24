#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern int cfd;

//接收一个条消息
struct MSG GetMSG()
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
		sigfun();
		return msgbuf;
	}

	return msgbuf;
}

