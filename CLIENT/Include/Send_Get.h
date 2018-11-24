#ifndef __SENDGET__
#define __SENDGET__
void SendMSG(struct MSG msgbuf, char s[]);
int GetAck();
struct MSG GetMSG();
#endif

