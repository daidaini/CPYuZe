#include "ConnHandle.h"
#include "StringFunc.h"
#include "VerifyHandle.h"
#include "StepPackage.h"

#define NonBlock

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

void ConnHandle::SetNonBlock()
{
#ifdef WIN32
	u_long iMode = 1; //非阻塞
	::ioctlsocket(sock, FIONBIO, &iMode);
#else
	int flags = fcntl(m_Socket, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(m_Socket, F_SETFL, flags);
#endif
}

#ifndef NonBlock
bool ConnHandle::Connect()
{
	auto addr_len = sizeof(sockaddr_in);

	/*linux下，阻塞情况默认超时时间为75秒
	*/
	int connect_ret = connect(m_Socket, (sockaddr*)&m_Addr, addr_len);
	if (connect_ret < 0)
	{
		Logger::GetInstance().Warn("[addr:{}:{}]connect failed", m_Ip, m_Port);
		return false;
	}

	int timeout = 10000; //10秒
	//发送超时设置
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (void*)&timeout, sizeof(int));
	//接收超时设置
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&timeout, sizeof(int));

	m_AliveTime = time(NULL);
	return true;
}
#else
bool ConnHandle::Connect()
{
	SetNonBlock();

	auto addr_len = sizeof(sockaddr_in);
	int rc = connect(m_Socket, (sockaddr*)&m_Addr, addr_len);
	Logger::GetInstance().Warn("connect return:{}", rc);

	fd_set fdr, fdw;
	if(rc != 0)
	{
		if(errno == EINPROGRESS)
		{
			//正在处理连接
			FD_ZERO(&fdw);
			FD_ZERO(&fdr);
			FD_SET(m_Socket, &fdw);
			FD_SET(m_Socket, &fdr);
			timeval timeout{10,0}; //10秒超时

			rc = select(m_Socket + 1, &fdr, &fdw, nullptr, &timeout);
			if(rc < 0)
			{
				Logger::GetInstance().Warn("connect error:{}", strerror(errno));
				return false;
			}

			if(rc == 0)
			{
				Logger::GetInstance().Warn("connect timeout");
				return false;
			}

			//当连接成功时，描述符变为可写，rc=1
			if(rc == 1 && FD_ISSET(m_Socket, &fdw))
			{
				m_AliveTime = time(NULL);
				return true;
			}

			if(rc == 2)
			{
				int err = 0;
				auto errlen = sizeof(err);
				if(getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, &err, (socklen_t*)&errlen) == -1)
				{
					Logger::GetInstance().Warn("getsockopt(SO_ERROR):{}", strerror(errno));
					return false;
				}
				if(err != 0)
				{
					errno = err;
					Logger::GetInstance().Warn("connect failed1:{}", strerror(errno));
					return false;
				}
			}
		}
		Logger::GetInstance().Warn("connect failed2:{}", strerror(errno));
		return false;
	}
	
	return true;
}

#endif

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
bool ConnHandle::DoRequest(const string& request)
{
    fd_set fs_read;
    FD_ZERO(&fs_read);
    timeval timeout{ 0, 100 };

	if(!SendRequest(request))
	{
		return false;
	}

    for (;;)
    {
        FD_SET(m_Socket, &fs_read);
        int select_ret = select(m_Socket + 1, &fs_read, nullptr, nullptr, &timeout);
        
        if (select_ret <= 0)
        {
            Logger::GetInstance().Warn("[index:{}] select error : {}", m_ReqIndex, strerror(errno));
            continue;
        }

        if (FD_ISSET(m_Socket, &fs_read))
        {
            char buf[2048]{};
            int ret = recv(m_Socket, buf, sizeof(buf) - 1, 0);
            if (ret <= 0)
            {
				Logger::GetInstance().Warn("[index:{}] recv happen some error: {}", m_ReqIndex, strerror(errno));
                return false;
            }
			Logger::GetInstance().Info("[index:{}] {}", m_ReqIndex, buf);
            DealAnswer(buf);
            FD_CLR(m_Socket, &fs_read);
			return true;
        }
    }
}

#endif // !NonBlock


void ConnHandle::DealAnswer(const string& rsp)
{
	ErrorInfo errs = StepPackage::ParseStepRsp(rsp);

	VerifyHandle::GetInstance().OnResult(errs.ErrCode, errs.ErrMsg, m_ReqIndex);
}