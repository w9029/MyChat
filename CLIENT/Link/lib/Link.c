#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Send_Get.h"

extern int cfd;

void Link()
{
	struct sockaddr_in seraddr;
	struct MSG msgbuf;
	int ret;

	signal(SIGINT, sigfun);
	
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(cfd < 0)
	{
		perror("socket error\n");
		exit(1);
	}
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(SPORT);
	seraddr.sin_addr.s_addr = inet_addr(SADDR);

	ret = connect(cfd, (struct sockaddr*)&seraddr, sizeof(struct sockaddr));
	if(ret != 0)
	{
		perror("connect error\n");
		sigfun();
	}

	ret = GetAck();
	//test link
	if(ret == 1)
	{
		printf("connect success!\n");
	}
	
	/*
	while(1)
	{
		int ret;
		struct MSG msgbuf;
		
		bzero(&msgbuf, sizeof(struct MSG));
		fgets(msgbuf.MSG_data, sizeof(msgbuf.MSG_data)-1, stdin);
		if(strlen(buff) > 0)
		{
			
			ret = send(cfd, )
		}
	}
	*/
}


