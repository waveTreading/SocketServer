#define WIN32_LEAN_AND_MEAN   // �������������������
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

int main()
{
	// ����Windows socket 2.x����
	WORD ver = MAKEWORD(2,2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//----------
	//-- ��Socket API��������TCP�ͻ���
	// 1 ����һ��socket
	// 2 ���ӷ����� connect
	// 3 ���շ�������Ϣ recv
	// 4 �ر��׽���closesocket
	//-- ��Socket API��������TCP�����
	// 1 ����һ��socket
	// 2 bind �����ڽ��տͻ������Ӷ�����˿�
	// 3 listen ��������˿�
	// 4 accept �ȴ����ܿͻ�������
	// 5 send ��ͻ��˷���һ������
	// 6 �ر��׽���closesocket
	//----------
	//���Windows socket����
	WSACleanup();
	return 0;
}