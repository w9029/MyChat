#include "include.h"
#include "Net_Link.h"
#include "MSG_struct.h"
#include "Send_Get.h"

extern int cfd;
extern char Myname[50];

void Logout()
{
	struct MSG msgbuf;

	bzero(&msgbuf, sizeof(struct MSG));
	msgbuf.MSG_TYPE = MSG_EXIT;
	SendMSG(msgbuf, "send exit error\n");
	strcpy(Myname, "");
}
