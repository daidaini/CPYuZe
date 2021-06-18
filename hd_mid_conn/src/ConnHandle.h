#pragma once

#ifdef WIN32
	#include<WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/select.h> 
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <arpa/inet.h>
	#include <poll.h>
	#include <errno.h>
#endif // WIN32


#include <string>
#include <time.h>

#include "Logger.h"

using namespace std;
class ConnHandle
{
public:
	ConnHandle(string addr, int port);
	~ConnHandle();

	bool Connect();
	/*返回false需要关闭连接
	*/
	bool DoRequest(const string& request);
	void SetRequestIndex(int index);
	int GetTimeInterval();

private:
	void Prepare();

	void DealAnswer(const string& rsp);

	void RefreshAliveTime();
	
	bool SendRequest(const string& request);
	bool RecvResponse(char* recvBuf, int recvBufLen);

	void SetNonBlock();

private:
	string m_Ip;
	int m_Port;

	int m_Socket;

	sockaddr_in m_Addr;

	time_t m_AliveTime = -1;

	int m_ReqIndex = 0;
};
