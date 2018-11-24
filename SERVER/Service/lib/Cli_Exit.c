#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "Linker_Service.h"

extern struct Linker Linker_head;		//接入者链表
extern struct Linker *Last_Linker;		//指向最后一个接入者
extern int sfd;
extern int ret;
extern MYSQL *mysql;

void Ser_cliExit(int cfd)
{
	struct Linker *t, *temp;
	temp = &Linker_head;
	while(temp)
	{
		if(temp->next && temp->next->Linker_cfd == cfd)
		{
			t = temp->next;
			printf("%s was exit\n", t->Linker_username);
			temp->next = t->next;
			free(t);
			
			//如果最后一个被删除 需要改变Last_Linker
			if(temp->next == NULL)
				Last_Linker = temp;
			break;
		}
		temp = temp->next;
	}
	SendAck(cfd);
}
