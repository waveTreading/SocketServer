#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN   // 减少其他依赖库的引用
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_ERROR
};

//消息头
struct DataHeader
{
	short dataLength;
	short cmd;
};

struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
	}
	int result;
};

struct LoginOut : public DataHeader
{
	LoginOut()
	{
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginOutResult : public DataHeader
{
	LoginOutResult()
	{
		dataLength = sizeof(LoginOutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 0;
	}
	int result;
};
// 接收数据信息的结构体
struct DataPackage
{
	int age;
	char name[32];
};
int main()
{
	// 启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--------
	//-- 用socket API建立建议TCP客户端
	// 1 建立一个Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0); // 第一个参数是 ipv4的套接字类型 第二个参数是面向流的套接字类型 第三是什么样的协议
	if (INVALID_SOCKET == _sock)
	{
		printf("ERROR，创建套接字失败...\n");
	}
	else
	{
		printf("建立socket成功...\n");
	}
	// 2 连接服务器 connet
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(8080);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock,(sockaddr*)&_sin,sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		printf("ERROR，建立连接失败...\n");
	}
	else
	{
		printf("建立连接成功");
	}
	while (true)
	{
		printf("请输入命令");
		// 3输入请求命令
		char cmdBuf[256] = {0};
		scanf("%s", cmdBuf);
		// 4处理请求命令
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("收到退出，任务结束");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "mima");
			// 5向服务器发送请求命令
			send(_sock, (const char *)&login, sizeof(login), 0);
			//接收服务器返回数据
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult: %d \n",loginRet.cmd);

		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LoginOut logOut;
			strcpy(logOut.userName, "lyd");
			// 5向服务器发送请求命令
			send(_sock, (const char*)&logOut, sizeof(logOut),0);
			//接收服务器返回数据
			LoginOutResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginOutResult: %d \n", loginRet.result);

		}
		else
		{
			printf("不支持命令，请重新输入。");
		}
	} 
	
	// 7 关闭套接字clostsocket
	closesocket(_sock);
	//清除Windows socket环境
	//--------
	WSACleanup();
	printf("已退出。");
	getchar();
	return 0;
}