#ifndef __NETLINK__
#define __NETLINK__

#include<pthread.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include "MSG_struct.h"

#define SPORT 6000
#define SADDR "0.0.0.0"

void Link();
void SendAck(int cfd);
void SendFail(int cfd, char s[]);
void SendMSG(int cfd, struct MSG msgbuf);
struct MSG GetMSG(int cfd);
#endif
