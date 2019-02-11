#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN   // 减少其他依赖库的引用
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_NEW_USER_JOIN,
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
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socket = 0;
	}
	int socket;
};
// 接收数据信息的结构体
struct DataPackage
{
	int age;
	char name[32];
};

int processor(SOCKET _cSock)
{
	// 缓冲区
	char szRecv[4096] = {};
	// 5 接收服务端信息
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("服务端  退出");
		return -1;
	};
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, (char*)&szRecv + sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
			LoginResult* loginResult = (LoginResult*)szRecv;
			printf("收到服务器消息 : CMD_LOGIN_RESULT: ,数据长度：%d \n", loginResult->dataLength);
		}
			break;
		case CMD_LOGINOUT_RESULT:
		{
			recv(_cSock, (char*)&szRecv + sizeof(DataHeader), sizeof(LoginOut)-sizeof(DataHeader), 0);
			LoginOutResult* loginResult = (LoginOutResult*)szRecv;
			printf("收到服务器消息 : CMD_LOGINOUT_RESULT: ,数据长度：%d \n", loginResult->dataLength);
		}
			break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, (char*)&szRecv + sizeof(DataHeader), sizeof(LoginOut)-sizeof(DataHeader), 0);
			NewUserJoin* newUserJoin = (NewUserJoin*)szRecv;
			printf("收到服务器消息 : CMD_NEW_USER_JOIN: ,数据长度：%d \n", newUserJoin->dataLength);
		}
	}
}

bool g_bRun = true;
//线程回调函数
void cmdThread(SOCKET _sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("退出cmdThread线程 \n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "lyd");
			send(_sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(cmdBuf, "loginout"))
		{
			LoginOut login;
			strcpy(login.userName, "lyd");
			send(_sock, (const char*)&login, sizeof(Login), 0);
		}
	}
	

}
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
	 
	//启动线程函数
	std::thread t1(cmdThread, _sock);
	t1.detach();
	while (g_bRun)
	{
		// 伯克利 socket
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		//nfds 是一个整数值 是指fd_set集合中所有描述符（socket）的范围，而不是数量
		// 即使所有文件描述符最大值+1， 在Windows中这个参数可以写0；
		timeval t = { 2, 0 };
		int ret = select(0, &fdReads, NULL, NULL, &t);
		if (ret < 0)
		{
			printf("select任务结束1 \n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock))
			{
				printf("select任务结束2 \n");
				break;
			}
		}
		printf("空闲时间处理其他业务 \n");
		Sleep(1000);
		Login login;
		strcpy(login.userName, "lyd");
		strcpy(login.passWord, "mima");
		// 5向服务器发送请求命令
		send(_sock, (const char *)&login, sizeof(login), 0);
		LoginOut logOut;
		strcpy(logOut.userName, "lyd");
		// 5向服务器发送请求命令
		send(_sock, (const char*)&logOut, sizeof(logOut), 0);
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