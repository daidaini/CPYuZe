#include "ConnHandle.h"
#include "StringFunc.h"
#include "VerifyHandle.h"
#include "StepPackage.h"

ConnHandle::ConnHandle(string addr, int port):
	m_Ip(addr), m_Port(port)
{
	Prepare();
}

void ConnHandle::Prepare()
{
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);

	m_Addr.sin_family = AF_INET;
	m_Addr.sin_port = htons(m_Port);
	m_Addr.sin_addr.s_addr = inet_addr(m_Ip.c_str());
}

ConnHandle::~ConnHandle()
{
#ifdef WIN32
	closesocket(m_Socket);
#else
	close(m_Socket);
#endif
}

bool ConnHandle::Connect()
{
	auto addr_len = sizeof(sockaddr_in);

	int connect_ret = connect(m_Socket, (sockaddr*)&m_Addr, addr_len);
	if (connect_ret < 0)
	{
		return false;
	}

	m_AliveTime = time(NULL);
	return true;
}

void ConnHandle::RefreshAliveTime()
{
	m_AliveTime = time(NULL);
}

int ConnHandle::GetTimeInterval()
{
	return static_cast<int>(time(NULL) - m_AliveTime);
}

void ConnHandle::SetRequestIndex(int index)
{
	m_ReqIndex = index;
}

bool ConnHandle::SendRequest(const string& request)
{
	char sendBuf[2048]{};
	int sendLen = 0;

	StepPackage::FillHeadForRequest(request, sendBuf, sendLen);
#ifdef WIN32
	int send_ret = ::send(m_Socket, sendBuf, sendLen, 0);
#else
	int send_ret = ::send(m_Socket, sendBuf, sendLen, MSG_NOSIGNAL);
#endif
	if (send_ret == 0)
	{
		Logger::GetInstance().Warn("[Index:{}]Send request failed", m_ReqIndex);
	}
	else if (send_ret < 0)
	{
#ifdef WIN32
		int lastError = GetLastError();
		Logger::GetInstance().Warn("[Index:{}]Send request failed, last error no = {}", m_ReqIndex, lastError);
#else
		Logger::GetInstance().Warn("[Index:{}]Send request failed, last error msg = {}", m_ReqIndex, strerror(errno));
#endif
	}

	if (send_ret <= 0)
	{
		VerifyHandle::GetInstance().OnResult(ErrCodeEnum::NetError,"socket send failed", m_ReqIndex);
		return false;
	}

	return true;
}

bool ConnHandle::RecvResponse(char* recvBuf, int recvBufLen)
{
	int recv_len = ::recv(m_Socket, recvBuf, recvBufLen, 0);

	if (recv_len == 0)
	{
		Logger::GetInstance().Warn("[Index:{}]Recv data failed", m_ReqIndex);
	}
	else if (recv_len < 0)
	{
#ifdef WIN32
		int lastError = GetLastError();
		Logger::GetInstance().Warn("[Index:{}]Recv data failed, last error no = {}", m_ReqIndex, lastError);
#else
		Logger::GetInstance().Warn("[Index:{}]Recv data failed, last error msg = {}", m_ReqIndex, strerror(errno));
#endif
	}

	if (recv_len <= 0)
	{
		VerifyHandle::GetInstance().OnResult(ErrCodeEnum::NetError, "socket recv failed", m_ReqIndex);
		return false;
	}

	if (recv_len >= recvBufLen)
	{
		//暂不处理
		Logger::GetInstance().Warn("[Index:{}]Recv not finished {}", m_ReqIndex);
	}

	return true;
}


//阻塞写法
#ifndef NonBlock
bool ConnHandle::DoRequest(const string& request)
{
	Logger::GetInstance().Info("[index:{}] {}", m_ReqIndex, request);

	RefreshAliveTime();

	if (!SendRequest(request))
	{
		return false;
	}

	char buf[2048]{};
	if (!RecvResponse(buf, sizeof(buf)))
	{
		return false;
	}

	Logger::GetInstance().Info("[index:{}] {}", m_ReqIndex, buf);
	DealAnswer(buf);
	return true;
}
#else
/*select写法*/
//
//void ConnHandle::DoRequest(string request)
//{
//    fd_set fs_read;
//    fd_set fs_write;
//    FD_ZERO(&fs_read);
//    FD_ZERO(&fs_write);
//    timeval timeout{ 0, 100 };
//
//    for (;;)
//    {
//        FD_SET(m_Socket, &fs_read);
//        FD_SET(m_Socket, &fs_write);
//        int select_ret = select(m_Socket + 1, &fs_read, &fs_write, NULL, &timeout);
//        
//        if (select_ret <= 0)
//        {
//            cout << "select error\n";
//            
//            continue;
//        }
//
//        if (FD_ISSET(m_Socket, &fs_read))
//        {
//            char buf[2048]{};
//            int ret = recv(m_Socket, buf, sizeof(buf) - 1, 0);
//            if (ret <= 0)
//            {
//                cout << "recv happen some error\n";
//                return;
//            }
//            DealAnswer(buf);
//            FD_CLR(m_Socket, &fs_read);
//        }
//        else if (!VerifyHandle::GetInstance().IsEmpty() && FD_ISSET(m_Socket, &fs_write))
//        {
//            auto req = VerifyHandle::GetInstance().GetRequest();
//            if (!req.empty())
//            {
//                int ret = send(m_Socket, req.c_str(), req.length(), 0);
//                if (ret < 0)
//                {
//                    cout << "send happen some error\n";
//                    return;
//                }
//                else
//                {
//                }
//            }
//            FD_CLR(m_Socket, &fs_write);
//            this_thread::sleep_for(chrono::milliseconds(10));
//        }   
//    }
//}

#endif // !NonBlock


void ConnHandle::DealAnswer(const string& rsp)
{
	ErrorInfo errs = StepPackage::ParseStepRsp(rsp);

	VerifyHandle::GetInstance().OnResult(errs.ErrCode, errs.ErrMsg, m_ReqIndex);
}