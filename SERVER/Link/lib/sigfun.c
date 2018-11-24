#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"

extern struct Linker Linker_head;	//接入者链表
extern struct Linker *Last_Linker;	//指向最后一个接入者
extern int sfd;
extern MYSQL *mysql;

void sigfun()
{
	close(sfd);
	mysql_close(mysql);
	exit(0);
}
