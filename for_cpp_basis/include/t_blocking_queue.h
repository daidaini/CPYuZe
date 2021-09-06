#pragma once

#include "comm.h"

template <typename T>
class BlockingQueue
{
public:
	BlockingQueue() = default;
	~BlockingQueue() = default;

	BlockingQueue(const BlockingQueue &rhs) = delete;
	BlockingQueue &operator=(const BlockingQueue &rhs) = delete;

	void Push(const T &task)
	{
		lock_guard<mutex> guard(m_Mt);
		m_Queue.emplace(std::move(task));
		cout << "Push(const T &task)\n";
		m_Cv.notify_one();
	}

	void Push(const T &&task)
	{
		lock_guard<mutex> guard(m_Mt);
		m_Queue.emplace(task);
		cout << "Push(const T &&task)\n";
		m_Cv.notify_one();
	}

	T Get()
	{
		unique_lock<mutex> dtLock(m_Mt);
		while (m_Queue.empty())
		{
			m_Cv.wait(dtLock);
		}
		assert(!m_Queue.empty());

		cout << "queue get begin\n";
		T data = std::move(m_Queue.front());
		m_Queue.pop();
		return data;
	}

	size_t size() const
	{
		lock_guard<mutex> guard(m_Mt);
		return m_Queue.size();
	}

private:
	mutex m_Mt;
	condition_variable m_Cv;
	queue<T> m_Queue;
};

struct TestData
{
	TestData(const string& data):_data(data)
	{

	}
	~TestData() = default;

	TestData(const TestData& d)
	{
		cout << "拷贝构造\n";
		_data = d._data;
	}
	TestData& operator=(TestData&) = default;

	/*
	TestData(TestData&& d)
	{
		cout << "移动构造\n";
		_data = d._data;
		d._data.clear();
	}

	TestData& operator=(TestData&& d)
	{
		cout << "移动赋值\n";
		_data = d._data;
		d._data.clear();
		return *this;
	}
	*/

	string _data;
};


inline namespace blocking_queue_test
{
	BlockingQueue<TestData> gQue;
	void thread_func_1()
	{
		for(int i=0; i < 10; ++i)
		{
			//gQue.Push(to_string(i));
			//string tmp("first");
			TestData t(to_string(i));
			gQue.Push(t);
			this_thread::sleep_for(10ms);
		} 
	}

	void thread_func_2()
	{
		for(int i=0; i < 10; ++i)
		{
			auto elem = gQue.Get();
			cout << elem._data <<endl;
			this_thread::sleep_for(20ms);
		} 
	}

	void bq_test()
	{
		cout << "begin test\n";

		thread(thread_func_1).detach();
		thread(thread_func_2).detach();

		cout << "end test\n";
	}
}