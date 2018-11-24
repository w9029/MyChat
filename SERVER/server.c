#include "include.h"
#include "Net_Link.h" 
#include "MSG_struct.h"
#include "Linker_struct.h"

struct Linker Linker_head;
struct Linker *Last_Linker;
int sfd, ret;
MYSQL *mysql;

int main()
{
	bzero(&Linker_head, sizeof(struct Linker));
	Link();
	return 0;
}

