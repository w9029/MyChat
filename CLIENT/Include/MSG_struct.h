#ifndef __MSG_STRUCT__
#define __MSG_STRUCT__

//types
#define MSG_ACK	1			//服务器肯定回应
#define MSG_FAIL 2			//服务器否定回应
#define MSG_LOGIN 3			//登陆请求
#define MSG_REGIST 4		//注册请求
#define MSG_CHPSWD 5		//更改密码请求
#define MSG_CHAT 6			//聊天信息，需要指定对象
#define MSG_LOCK 7			//禁言信息，需要验证权限
#define MSG_UNLOCK 8		//解禁信息，需要验证权限即禁言状态
#define MSG_CHECK_COUNT 9	//查看在线人数
#define MSG_KICK 10			//踢人
#define MSG_UNKICK 11		//拉回
#define MSG_CHAT_Private 12	//私聊请求
#define MSG_CHAT_ALL 13		//群聊请求
#define MSG_FILE_NAME 14	//文件传输-文件名
#define MSG_FILE_DATA 15	//文件传输-文件内容
#define MSG_FILE_DONE 16	//文件传输-完成
#define MSG_FILE_ERROR 17	//文件传输-异常终止
#define MSG_TEMP 18			//传输其他数据(帐号密码等)
#define MSG_EXIT 19			//用户退出信号

struct MSG
{
	int MSG_TYPE;
	char MSG_SRC[50];
	char MSG_DES[50];
	char MSG_data[1024];
};

#endif

