#include "include.h"
#include "Net_Link.h"
#include "Linker_struct.h"
#include "MSG_struct.h"
#include "sigfun.h"
#include "Linker_Service.h"

extern struct Linker Linker_head;	//接入者链表
extern struct Linker *Last_Linker;			//指向最后一个接入者
extern int sfd;
extern MYSQL *mysql;

void Link()
{
	Last_Linker = &Linker_head;
	Last_Linker->next = NULL;
	pthread_t tid;
	int opt = 1;
	struct sockaddr_in seraddr;
	int ret;

	signal(SIGINT, sigfun);
	
	//连接数据库
	mysql = mysql_init(NULL);
	if(!mysql)
	{
		printf("mysql init fail\n");
		exit(1);
	}
	ret = mysql_real_connect(mysql, SQL_HOST, SQL_USER, SQL_PSWD, SQL_DB_NAME, 0, NULL, 0);
	if(!ret)
	{
		printf("SQL connect fail\n");
		exit(1);
	}

	//创建服务器socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
	{
		perror("socket error\n");
		exit(1);
	}

	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(SPORT);
	seraddr.sin_addr.s_addr = inet_addr(SADDR);

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	ret = bind(sfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
	if(ret != 0)
	{
		perror("bind error\n");
		sigfun();
	}

	//设置最大监听数 50
	ret = listen(sfd, 50);
	if(ret != 0)
	{
		perror("listen error\n");
		sigfun();
	}

	while(1)
	{
		int cfd, len;
		struct sockaddr_in cliaddr;
		
		cfd = accept(sfd, (struct sockaddr*)&cliaddr, &len);
		printf("IP:%s connected\n", inet_ntoa(cliaddr.sin_addr));
		if(cfd > 0)
		{
			//创建线程
			pthread_create(&tid, NULL, Linker_Service, &cfd);
		}
	}
}
