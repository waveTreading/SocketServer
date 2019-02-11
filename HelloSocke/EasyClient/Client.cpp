#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN   // �������������������
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
// ����������Ϣ�Ľṹ��
struct DataPackage
{
	int age;
	char name[32];
};

int processor(SOCKET _cSock)
{
	// ������
	char szRecv[4096] = {};
	// 5 ���շ������Ϣ
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("�����  �˳�");
		return -1;
	};
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, (char*)&szRecv + sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
			LoginResult* loginResult = (LoginResult*)szRecv;
			printf("�յ���������Ϣ : CMD_LOGIN_RESULT: ,���ݳ��ȣ�%d \n", loginResult->dataLength);
		}
			break;
		case CMD_LOGINOUT_RESULT:
		{
			recv(_cSock, (char*)&szRecv + sizeof(DataHeader), sizeof(LoginOut)-sizeof(DataHeader), 0);
			LoginOutResult* loginResult = (LoginOutResult*)szRecv;
			printf("�յ���������Ϣ : CMD_LOGINOUT_RESULT: ,���ݳ��ȣ�%d \n", loginResult->dataLength);
		}
			break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, (char*)&szRecv + sizeof(DataHeader), sizeof(LoginOut)-sizeof(DataHeader), 0);
			NewUserJoin* newUserJoin = (NewUserJoin*)szRecv;
			printf("�յ���������Ϣ : CMD_NEW_USER_JOIN: ,���ݳ��ȣ�%d \n", newUserJoin->dataLength);
		}
	}
}

bool g_bRun = true;
//�̻߳ص�����
void cmdThread(SOCKET _sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳�cmdThread�߳� \n");
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
	 
	//�����̺߳���
	std::thread t1(cmdThread, _sock);
	t1.detach();
	while (g_bRun)
	{
		// ������ socket
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		//nfds ��һ������ֵ ��ָfd_set������������������socket���ķ�Χ������������
		// ��ʹ�����ļ����������ֵ+1�� ��Windows�������������д0��
		timeval t = { 2, 0 };
		int ret = select(0, &fdReads, NULL, NULL, &t);
		if (ret < 0)
		{
			printf("select�������1 \n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock))
			{
				printf("select�������2 \n");
				break;
			}
		}
		printf("����ʱ�䴦������ҵ�� \n");
		Sleep(1000);
		Login login;
		strcpy(login.userName, "lyd");
		strcpy(login.passWord, "mima");
		// 5�������������������
		send(_sock, (const char *)&login, sizeof(login), 0);
		LoginOut logOut;
		strcpy(logOut.userName, "lyd");
		// 5�������������������
		send(_sock, (const char*)&logOut, sizeof(logOut), 0);
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