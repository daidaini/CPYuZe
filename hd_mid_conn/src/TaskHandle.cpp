#include "TaskHandle.h"
#include "Logger.h"

using namespace std;
TaskHandle::TaskHandle(int fixThreadCnt, int maxThreadCount) :
	m_FixedThreadCnt(fixThreadCnt),m_MaxThreadCnt(maxThreadCount)
{
	m_ThreadPools.emplace_back(make_shared<FixedThreadPool>(fixThreadCnt));
}



void TaskHandle::ExecuteTask(FuncType&& task)
{
	CheckThreadPools();
	for(int i=0; i< m_ThreadPools.size(); ++i)
	{
		if (!m_ThreadPools[i]->all_occupied())
		{
			m_ThreadPools[i]->execute(std::forward<FuncType>(task));
			return;
		}
	}
	
	DealLeftTask(std::forward<FuncType>(task));
}

void TaskHandle::DealLeftTask(FuncType&& task)
{
	int nowThreadCnt = m_ThreadPools.size() * m_FixedThreadCnt;
	if (nowThreadCnt >= m_MaxThreadCnt)
	{
		m_ThreadPools[0]->execute(std::forward<FuncType>(task));
		Logger::GetInstance().Warn("threadpool is full£¬and task is delayed");
	}
	else
	{
		int cntToAdd = nowThreadCnt + m_FixedThreadCnt > m_MaxThreadCnt ? m_MaxThreadCnt - nowThreadCnt : m_FixedThreadCnt;
		auto pool = std::make_shared<FixedThreadPool>(cntToAdd);
		m_ThreadPools.emplace_back(pool);
		pool->execute(std::forward<FuncType>(task));
		Logger::GetInstance().Warn("create one new threadpool£¬threadpool count={}", m_ThreadPools.size());
	}
}

void TaskHandle::CheckThreadPools()
{
	if (m_ThreadPools.size() <= 1)
		return;

	for (auto it = m_ThreadPools.begin() + 1; it != m_ThreadPools.end();)
	{
		auto ptr = *it;
		if (ptr->unoccupied())
		{
			Logger::GetInstance().Warn("free threadpool£¬count={}", m_ThreadPools.size() - 1);
			it = m_ThreadPools.erase(it);
		}
		else
		{
			++it;
		}
	}
}
