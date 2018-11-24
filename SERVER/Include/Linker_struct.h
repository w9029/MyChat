#ifndef __LINKER__
#define __LINKER__

#define Linker_FREE 1
#define Linker_LOCK 2
#define Linker_KICK 3

struct Linker
{
	int Linker_cfd;
	char Linker_username[50];
	int Linker_stat;
	struct Linker *next;
};

#endif
