#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN   // �������������������
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

// ����������Ϣ�Ľṹ��
struct DataPackage
{
	int age;
	char name[32];
};
int main()
{
	// ����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//--------
	//-- ��socket API��������TCP�ͻ���
	// 1 ����һ��Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0); // ��һ�������� ipv4���׽������� �ڶ������������������׽������� ������ʲô����Э��
	if (INVALID_SOCKET == _sock)
	{
		printf("ERROR�������׽���ʧ��...\n");
	}
	else
	{
		printf("����socket�ɹ�...\n");
	}
	// 2 ���ӷ����� connet
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(8080);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock,(sockaddr*)&_sin,sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		printf("ERROR����������ʧ��...\n");
	}
	else
	{
		printf("�������ӳɹ�");
	}
	while (true)
	{
		printf("����������");
		// 3������������
		char cmdBuf[256] = {0};
		scanf("%s", cmdBuf);
		// 4������������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("�յ��˳����������");
			break;
		}
		else
		{
			// 5�������������������
			send(_sock, cmdBuf, sizeof(cmdBuf), 0);
		}

		// 6 ���շ�������Ϣ
		char recvBuf[256] = {0};
		int nlen = recv(_sock, recvBuf,sizeof(recvBuf), 0);
		if (nlen > 0)
		{
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("���յ������ݣ� ����:%d ����:%s \n",dp->age,dp->name);
		}
	} 
	
	// 7 �ر��׽���clostsocket
	closesocket(_sock);
	//���Windows socket����
	//--------
	WSACleanup();
	printf("���˳���");
	getchar();
	return 0;
}