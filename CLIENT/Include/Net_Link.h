#ifndef __NETLINK__
#define __NETLINK__

#include<pthread.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define SPORT 6001
#define SADDR "192.168.100.85"
//#define SADDR "10.158.61.107"
//#define SADDR "172.20.10.3"


void Link();

#endif
