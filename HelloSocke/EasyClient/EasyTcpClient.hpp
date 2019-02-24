#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <WinSock2.h>
	#pragma comment(lib,"ws2_32.lib")
#else
	#include<unistd.h> //uni std
	#include<arpa/inet.h>
	#include<string.h>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif
#include <stdio.h>
#include "MessageHeader.hpp"

class EasyTcpClient
{
	SOCKET _sock;

public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpClient()
	{
		
	}
	//初始化Socket
	void InitSocket()
	{
#ifdef _WIN32
		//启动Windows socket 2.x环境
		WORD ver = MAKEWORD(2,2); //通过连接指定值
		WSADATA dat;
		WSAStartup(ver,&dat); //异步套接字启动命令
#endif
		if (INVALID_SOCKET != _sock)
		{
			printf("<socket = $d>关闭连接", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			printf("错误， 建立socket失败... \n");
		}
		else
		{
			printf("建立socket成功...\n");
		}
	}
	//连接服务器
	int Connect(char* ip,unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		//连接服务器
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr  = inet_addr(ip);
#endif
		int ret = connect(_sock,(sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret )
		{
			printf("错误， 连接服务器失败....\n");
		}
		else
		{
			printf("连接服务器成功");
		}
		return ret;

	}
	//关闭套节字closesocket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sock);
			//清除Windows socket环境
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}
	}
	//处理网络信息
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock,&fdReads);
			timeval t = {1,0};
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0)
			{
				printf("<socket=%d>select任务结束1\n", _sock);
				return false;
			}
			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);

				if (-1 == RecvData(_sock))
				{
					printf("<socket=%d>select任务结束2\n", _sock);
					return false;
				}
			}
			return true;
		}
		return false;
	}
	//是否在工作
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}
	//接收数据 处理粘包 拆分包
	int RecvData(SOCKET _cSock)
	{
		//缓冲区
		char szRecv[4096] = {};
		//5 接收客户端数据
		int nLen = (int)recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen < 0)
		{
			printf("与服务器断开连接，任务结束。 \n");
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);

		OnNetMsg(header);
		return 0;

	}
	//响应网络消息
	void OnNetMsg(DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* login = (LoginResult*)header;
			printf("收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n", login->dataLength);
		}
			break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logout = (LogoutResult*)header;
			printf("收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", logout->dataLength);
		}
			break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* userJoin = (NewUserJoin*)header;
			printf("收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n", userJoin->dataLength);
		}
			break;
		}
	}
	//发送数据
	int SendData(DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
private:

};

#endif