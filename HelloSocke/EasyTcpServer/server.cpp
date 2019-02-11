#define WIN32_LEAN_AND_MEAN   // �������������������
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

#include <vector>

#pragma comment(lib,"ws2_32.lib")

using namespace std;
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

vector<SOCKET> g_clients;

int processor(SOCKET _cSock)
{
	// ������
	char szRecv[4096] = {};
	// 5 ���տͻ�������
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("�ͻ���  �˳�");
		return -1;
	};

	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
					  Login login = {};
					  recv(_cSock, (char*)&login + sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
					  printf("�յ�����:%d ���ݳ��ȣ�%d  �˺�%s   ���룺%s\n", login.cmd, login.dataLength, login.userName, login.passWord);
					  //�����ж��û������Ƿ���ȷ�Ĺ���
					  LoginResult ret;
					  send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
	}
		break;
	case CMD_LOGINOUT:
	{
						 LoginOut loginOut = {};
						 recv(_cSock, (char*)&loginOut + sizeof(DataHeader), sizeof(LoginOut)-sizeof(DataHeader), 0);
						 printf("�յ�����:%d ���ݳ��ȣ�%d  �˺�%s\n", loginOut.cmd, loginOut.dataLength, loginOut.userName);

						 //�����ж��û������Ƿ���ȷ�Ĺ���
						 LoginResult ret;
						 send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
	}
		break;
	default:
		header->cmd = CMD_ERROR;
		header->dataLength = 0;
		send(_cSock, (char*)&header, sizeof(header), 0);
		break;
	}
}

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
	while (true)
	{
		// ������ socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		//�����µĿͻ��˼����ʱ�� ������ӽ���socket��ӵ�fdREad��
		for (int i = (int)g_clients.size()-1; i >= 0; i--)
		{
			FD_SET(g_clients[i], &fdRead);
		}
		//nfds ��һ������ֵ ��ָfd_set������������������socket���ķ�Χ������������
		// ��ʹ�����ļ����������ֵ+1�� ��Windows�������������д0��
		int ret = select(0,&fdRead, &fdWrite, &fdExp, NULL);
		if (ret < 0)
		{
			printf("�ͻ������Ƴ������������ \n");
			break;
		}
		// �жϼ������Ƿ��пɲ���socket
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			// 4 accept �ȴ����ܿͻ�������
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;

			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("ERROT�����յ���Ч�ͻ���socket...\n");
			}
			printf("�¿ͻ��˼��룺 IP = %s \n", inet_ntoa(clientAddr.sin_addr)); // inet_ntoa ת���ɿɶ���
			g_clients.push_back(_cSock);
		}

		for (int n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				// fdRead���ص�socket��g_clients��Ѱ�� Ȼ��ɾ��
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}

		}
		
	}
	printf("�ͻ����Ѿ��˳������������");
	for (size_t n = g_clients.size(); n >= 0 ; n--)
	{
		closesocket(g_clients[n]);
	}
	closesocket(_sock);
	// 8 �ر��׽���closesocket
	//----------
	//���Windows socket����
	WSACleanup();
	return 0;
}