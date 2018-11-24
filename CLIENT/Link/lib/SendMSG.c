#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern int cfd;

void SendMSG(struct MSG msgbuf, char s[])
{
	int ret;
	
	ret = send(cfd, &msgbuf, sizeof(struct MSG), 0);
	if(ret < 0)
	{
		perror(s);
	}
	else if(ret == 0)
	{
		perror("Unconnected\n");
		sigfun();
	}
}



