#define WIN32_LEAN_AND_MEAN   // 减少其他依赖库的引用
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_ERROR
};

//消息头
struct DataHeader
{
	short dataLength;
	short cmd;
};

struct Login
{
	char userName[32];
	char passWord[32];
};

struct LoginResult
{
	int result;
};

struct LoginOut
{
	char userName[32];
};
struct LoginOutResult
{
	int result;
};

// 发送数据信息的结构体
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
	//----------
	//-- 用Socket API建立建议TCP服务端
	// 1 建立一个socket 套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 第一个参数是 ipv4的套接字类型 第二个参数是面向流的套接字类型 第三是什么样的协议
	// 2 bind 绑定用于接收客户端连接额网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(8080);// host to net unsigned short 主机到网络字节序的转换
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // ipv4的地址
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("ERROR,绑定网络端口失败...\n");
	}
	else
	{
		printf("绑定网络端口成功...\n");
	}
	// 3 listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR,监听网络端口失败...\n");
	}
	else
	{
		printf("监听网络端口成功...\n");
	}
	// 4 accept 等待接受客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;

	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("ERROT，接收到无效客户端socket...\n");
	}
	// 5 send 向客户端发送一条数据
	printf("新客户端加入： IP = %s \n", inet_ntoa(clientAddr.sin_addr)); // inet_ntoa 转换成可读的
	while (true)
	{
		DataHeader header = {};
		// 5 接收客户端数据
		int recLen = recv(_cSock,(char*)&header,sizeof(header),0);
		if (recLen < 0)
		{
			printf("客户端%s   退出", inet_ntoa(clientAddr.sin_addr));
			break;
		};
		printf("收到命令:%d 数据长度：%d\n", header.cmd, header.dataLength);
		switch (header.cmd)
		{
			case CMD_LOGIN:
			{
				Login login = {};
				recv(_cSock, (char*)&login,sizeof(Login), 0);
				//忽略判断用户密码是否正确的过程
				LoginResult ret = {0};
				send(_cSock, (char*)&header, sizeof(DataHeader), 0);
				send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
			}
			break;
			case CMD_LOGINOUT:
			{
				LoginOut loginOut = {};
				recv(_cSock, (char*)&loginOut, sizeof(LoginOut), 0);
				//忽略判断用户密码是否正确的过程
				LoginResult ret = {1};
				send(_cSock, (char*)&header, sizeof(DataHeader), 0);
				send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
			}
				break;
		default:
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_cSock, (char*)&header, sizeof(header), 0);
			break;
		}
	}
	printf("客户端已经退出，任务结束。");
	closesocket(_cSock);
	// 6 关闭套接字closesocket
	//----------
	//清除Windows socket环境
	WSACleanup();
	return 0;
}