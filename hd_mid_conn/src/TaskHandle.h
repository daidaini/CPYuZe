#pragma once

#include "FixThreadPool.h"

class TaskHandle
{
	using FuncType = std::function<void()>;
public:
	TaskHandle(int fixThreadCnt, int maxThreadCount);
	~TaskHandle() = default;

	void ExecuteTask( FuncType&& task);

private:
	void CheckThreadPools();

	void DealLeftTask(FuncType&& task);

	std::vector<std::shared_ptr<FixedThreadPool>> m_ThreadPools;
	
	int m_FixedThreadCnt;
	int m_MaxThreadCnt;
};


