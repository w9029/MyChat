#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"

extern int cfd;

void sigfun()
{
	struct MSG msgbuf;
	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_EXIT;
	send(cfd, &msgbuf, sizeof(struct MSG), 0);

	close(cfd);
	exit(0);
}

