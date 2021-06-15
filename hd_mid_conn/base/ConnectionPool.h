#pragma once

#include <memory>
#include <string>
#include <queue>
#include <mutex>

#include "ConnHandle.h"

using namespace std;

struct ConnectionPoolConfig
{
	string IP;
	int Port = 0;
	int Timeout = 0;
	int MaxConnCnt = 0;
};

class ConnectionPool
{
public:
	static ConnectionPool& GetInstance();

	shared_ptr<ConnHandle> PopConnection();
	void PushConnection(shared_ptr<ConnHandle> conn);
	void DecreaseConnectionCnt();

	void LoadConfig(const ConnectionPoolConfig& path);
private:
	ConnectionPool() = default;
	~ConnectionPool() = default;

	shared_ptr<ConnHandle> ProduceConnection();

	shared_ptr<ConnHandle> GetValidConn();

private:
	ConnectionPoolConfig m_Confs;

	int m_ConnCnt = 0;
	queue<shared_ptr<ConnHandle>> m_ConnQue;

	mutex m_Mt;
};