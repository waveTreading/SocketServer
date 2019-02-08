#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN   // 减少其他依赖库的引用
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

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
		else
		{
			// 5向服务器发送请求命令
			send(_sock, cmdBuf, sizeof(cmdBuf), 0);
		}

		// 6 接收服务器信息
		char recvBuf[256] = {0};
		int nlen = recv(_sock, recvBuf,sizeof(recvBuf), 0);
		if (nlen > 0)
		{
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("接收到的数据： 年龄:%d 姓名:%s \n",dp->age,dp->name);
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