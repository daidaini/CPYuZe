#include "VerifyHandle.h"
#include "json/json.h"
#include "Utility.h"

VerifyHandle& VerifyHandle::GetInstance()
{
	static VerifyHandle instance;
	return instance;
}

VerifyHandle::VerifyHandle()
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		Logger::GetInstance().Warn("ÍøÂç¿â³õÊ¼»¯Ê§°Ü");
	}
#endif
}

VerifyHandle::~VerifyHandle()
{
#ifdef WIN32
	WSACleanup();
#endif
}

void VerifyHandle::SetCallback(CallbackType func)
{
	m_RspFunc = func;
}

bool VerifyHandle::Init()
{
	string configPath = "./hd_mid_conf.json";
	string contents = Utility::ReadContentsFromFile(configPath);
	if (contents.empty())
	{
		OnResult(ErrCodeEnum::OtherError, "read config contents failed[check config path]"s, -1);
		return false;
	}

	Json::Value jval;
	Json::Reader jreader;
	if (!jreader.parse(contents, jval))
	{
		OnResult(ErrCodeEnum::OtherError, "config format happened some error"s, -1);
		return false;
	}

	try
	{
		ConnectionPoolConfig cpConf{};
		cpConf.IP = jval["ip"].asString();
		cpConf.Port = jval["port"].asInt();
		cpConf.MaxConnCnt = jval["connection_pool"]["max_connection_count"].asInt();
		cpConf.Timeout = jval["connection_pool"]["connection_timeout"].asInt();

		int thread_count = jval["thread_pool"]["fixed_thread_count"].asInt();
		int max_thread_count = jval["thread_pool"]["max_thread_count"].asInt();

		ConnectionPool::GetInstance().LoadConfig(cpConf);
		m_TaskHandle = make_unique<TaskHandle>(thread_count, max_thread_count);

		Logger::GetInstance().Warn("config:ip={}, port={}", cpConf.IP, cpConf.Port);
		Logger::GetInstance().Warn("config:[connection_pool]max_connection_num={},connection_timeout={}", cpConf.MaxConnCnt, cpConf.Timeout);
		Logger::GetInstance().Warn("config:[thread_pool]fixed_thread_num={}", thread_count);
	}
	catch (const std::exception& e)
	{
		OnResult(ErrCodeEnum::OtherError, "config data format happened some error"s, -1);
		Logger::GetInstance().Warn("????{}??:[{}]", configPath, e.what());
		return false;
	}
	return true;
}

void VerifyHandle::AddRequest(int index, const string& request)
{
	m_TaskHandle->ExecuteTask(std::bind(&VerifyHandle::HandleRequestTask, this, index, request));
}


void VerifyHandle::HandleRequestTask(int index, const string request)
{
	auto conn = ConnectionPool::GetInstance().PopConnection();
	
	if (conn == nullptr)
	{
		unique_lock<mutex> lock(m_Mt);
		std::cv_status status = m_Cv.wait_for(lock, chrono::seconds(m_TaskConnTimeOut));
		if(status == cv_status::timeout)
		{
			OnResult(ErrCodeEnum::OtherError, "connect overtime error"s, -1);
			Logger::GetInstance().Warn("Get connection from connect pool failed[overtime]");
			return;
		}
		conn = ConnectionPool::GetInstance().PopConnection();
	}

	conn->SetRequestIndex(index);
	if (conn->DoRequest(request))
	{
		unique_lock<mutex> lock(m_Mt);
		ConnectionPool::GetInstance().PushConnection(conn);
		m_Cv.notify_one();
	}
	else
	{
		ConnectionPool::GetInstance().DecreaseConnectionCnt();
	}
}

void VerifyHandle::OnResult(ErrCodeEnum errcode, const string& errmsg, int index)
{
	ErrorInfo err{};
	err.ErrCode = errcode;
	strncpy(err.ErrMsg, errmsg.c_str(), sizeof(err.ErrMsg));

	m_RspFunc(&err, index);
}