#define WIN32_LEAN_AND_MEAN   // �������������������
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

//��Ϣͷ
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
	//----------
	//-- ��Socket API��������TCP�����
	// 1 ����һ��socket �׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // ��һ�������� ipv4���׽������� �ڶ������������������׽������� ������ʲô����Э��
	// 2 bind �����ڽ��տͻ������Ӷ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(8080);// host to net unsigned short �����������ֽ����ת��
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // ipv4�ĵ�ַ
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("ERROR,������˿�ʧ��...\n");
	}
	else
	{
		printf("������˿ڳɹ�...\n");
	}
	// 3 listen ��������˿�
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR,��������˿�ʧ��...\n");
	}
	else
	{
		printf("��������˿ڳɹ�...\n");
	}
	// 4 accept �ȴ����ܿͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;

	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("ERROT�����յ���Ч�ͻ���socket...\n");
	}
	// 5 send ��ͻ��˷���һ������
	printf("�¿ͻ��˼��룺 IP = %s \n", inet_ntoa(clientAddr.sin_addr)); // inet_ntoa ת���ɿɶ���
	while (true)
	{
		DataHeader header = {};
		// 5 ���տͻ�������
		int recLen = recv(_cSock,(char*)&header,sizeof(header),0);
		if (recLen < 0)
		{
			printf("�ͻ���%s   �˳�", inet_ntoa(clientAddr.sin_addr));
			break;
		};
		printf("�յ�����:%d ���ݳ��ȣ�%d\n", header.cmd, header.dataLength);
		switch (header.cmd)
		{
			case CMD_LOGIN:
			{
				Login login = {};
				recv(_cSock, (char*)&login,sizeof(Login), 0);
				//�����ж��û������Ƿ���ȷ�Ĺ���
				LoginResult ret = {0};
				send(_cSock, (char*)&header, sizeof(DataHeader), 0);
				send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
			}
			break;
			case CMD_LOGINOUT:
			{
				LoginOut loginOut = {};
				recv(_cSock, (char*)&loginOut, sizeof(LoginOut), 0);
				//�����ж��û������Ƿ���ȷ�Ĺ���
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
	printf("�ͻ����Ѿ��˳������������");
	closesocket(_cSock);
	// 6 �ر��׽���closesocket
	//----------
	//���Windows socket����
	WSACleanup();
	return 0;
}