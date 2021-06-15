#include "ConnectionPool.h"
#include "Utility.h"
#include "json/json.h"
#include "Logger.h"

ConnectionPool& ConnectionPool::GetInstance()
{
	static ConnectionPool instance;
	return instance;
}

void ConnectionPool::LoadConfig(const ConnectionPoolConfig& conf)
{
	m_Confs.IP = conf.IP;
	m_Confs.Port = conf.Port;
	m_Confs.MaxConnCnt = conf.MaxConnCnt;
	m_Confs.Timeout = conf.Timeout;
}

shared_ptr<ConnHandle> ConnectionPool::ProduceConnection()
{
	if (m_ConnCnt >= m_Confs.MaxConnCnt)
		return nullptr;

	auto conn = make_shared<ConnHandle>(m_Confs.IP, m_Confs.Port);
	
	if (conn == nullptr)
		return nullptr;

	if (!conn->Connect())
		return nullptr;

	++m_ConnCnt;
	//Logger::GetInstance().Warn("create one connect: connect count={}", m_ConnCnt);
	return conn;
}

shared_ptr<ConnHandle> ConnectionPool::GetValidConn()
{
	lock_guard<mutex> guard(m_Mt);
	
	shared_ptr<ConnHandle> conn = nullptr;
	while (!m_ConnQue.empty())
	{
		conn = m_ConnQue.front();
		m_ConnQue.pop();
		if (conn->GetTimeInterval() > m_Confs.Timeout)
		{
			//≥¨ ±£¨ Õ∑≈
			conn = nullptr;
			--m_ConnCnt;
			Logger::GetInstance().Warn("connect overtime, need to free : connect count={}", m_ConnCnt);
		}
		else
		{
			break;
		}
	}
	
	if (conn == nullptr)
	{
		conn = ProduceConnection();
	}

	return conn;
}

shared_ptr<ConnHandle> ConnectionPool::PopConnection()
{
	auto conn = GetValidConn();
	if (conn == nullptr)
		return nullptr;

	return conn;
}

void ConnectionPool::PushConnection(shared_ptr<ConnHandle> conn)
{
	lock_guard<mutex> guard(m_Mt);
	if (conn != nullptr)
	{
		m_ConnQue.push(conn);
	}
}

void ConnectionPool::DecreaseConnectionCnt()
{
	--m_ConnCnt;
	//Logger::GetInstance().Warn("free one connect : connect count={}", m_ConnCnt);
}
