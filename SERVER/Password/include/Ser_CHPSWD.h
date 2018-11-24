#ifndef __CHPSWD__
#define __CHPSWD__
void Ser_CHPSWD(int cfd);
void Ch_by_PW(int cfd, char username[]);
void Ch_by_Q(int cfd, char username[]);
int Get_Set_newpasswd(int cfd, char username[]);
#endif

