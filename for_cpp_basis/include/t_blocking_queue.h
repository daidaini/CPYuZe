#pragma once 

#include "comm.h"

template<typename T>
class BlockingQueue
{
public:
	BlockingQueue() = default;
	~BlockingQueue() = default;

	void Push(const T& x)
	{
		lock_guard<mutex> guard(_mt);
		_queue.push_back(x);
		_cv.notify_one();
	}

	void Push(const T&& x)
	{
		lock_guard<mutex> guard(_mt);
		_queue.push_back(std::move(x));
		_cv.notify_one();
	}

	T PopFront()
	{
		unique_lock<mutex> lock(_mt);
		while (_queue.empty())
		{
			_cv.wait(lock);
		}
		assert(!_queue.empty());
		
		T front(std::move(_queue.front()));
		_queue.pop_front();
		return front;
	}

	size_t size()
	{
		lock_guard<mutex> guard(_mt);
		return _queue.size();
	}

private:
	mutex _mt;
	condition_variable _cv;
	queue<T> _queue;
};