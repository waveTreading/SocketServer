#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN   // �������������������
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

//��Ϣͷ
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
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "lyd");
			strcpy(login.passWord, "mima");
			// 5�������������������
			send(_sock, (const char *)&login, sizeof(login), 0);
			//���շ�������������
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult: %d \n",loginRet.cmd);

		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LoginOut logOut;
			strcpy(logOut.userName, "lyd");
			// 5�������������������
			send(_sock, (const char*)&logOut, sizeof(logOut),0);
			//���շ�������������
			LoginOutResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginOutResult: %d \n", loginRet.result);

		}
		else
		{
			printf("��֧��������������롣");
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