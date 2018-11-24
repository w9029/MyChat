#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"

extern struct Linker Linker_head;	//在线人链表头
extern struct Linker *Last_Linker;	//指向最后一个在线人
extern int sfd;

int Get_user_cfd(char username[])
{
	struct Linker *temp;
	temp = Linker_head.next;
	while(temp)
	{
		if(strcmp(username, temp->Linker_username) == 0)
		{
			return temp->Linker_cfd;
		}
		temp = temp->next;
	}
	return -1;
}

