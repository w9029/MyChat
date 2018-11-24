#include "include.h"
#include "Net_Link.h"
#include "Login.h"
#include "Regist.h"

void MyChatInterface()
{
	char flag = 0;
	while(flag != '4')
	{
		fflush(stdin);
		system("clear");
		printf("****** MyChat ******\n");
		printf("*                  *\n");
		printf("*   1.登录MyChat   *\n");
		printf("*                  *\n");
		printf("*   2.注册用户     *\n");
		printf("*                  *\n");
		printf("*   3.修改密码     *\n");
		printf("*                  *\n");
		printf("*   4.退出系统     *\n");
		printf("*                  *\n");
		printf("********************\n");
		printf("请输入您的选择:\n");

		scanf("%c",&flag);
		fflush(stdin);
		
		switch(flag)
		{
			case '1':
				//登录
				Login();
				break;
			case '2':
				//注册
				Regist();
				break;
			case '3':
				//修改密码
				Ch_Passwd();
				break;
			case '4':
				//退出
				printf("exit\n");
				break;
			default:
				printf("input error, please try again\n");
				break;
		}	
	}
}
