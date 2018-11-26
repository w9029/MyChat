#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern int cfd;

//接收一个ack应答信息
int GetAck()
{
	int ret;
	struct MSG msgbuf;
	bzero(&msgbuf, sizeof(struct MSG));

	ret = recv(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret < 0)
	{
		perror("recv ack error\n");
		return 0;
	}
	else if(ret == 0)
	{
		printf("connection is broken!\n");
		sigfun();
		return 0;
	}
	else if(msgbuf.MSG_TYPE == MSG_ACK 
			|| msgbuf.MSG_TYPE == MSG_FAIL)
	{
		if(msgbuf.MSG_TYPE == MSG_FAIL)
			printf("%s\n",msgbuf.MSG_data);

		return msgbuf.MSG_TYPE;
	}
	else 
	{
		printf("recvived data is not ACK! %d \n", msgbuf.MSG_TYPE);
		return 0;
	}
}

