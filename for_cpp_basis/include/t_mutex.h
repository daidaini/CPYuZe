#pragma once

#include "comm.h"


class T_Mutex
{
public:
	T_Mutex() {

	}

	~T_Mutex() {

	}

	void Comsumpe()
	{
		while (true)
		{
			if (!_tmx.try_lock_for(20ms))
			{
				cout << "timed_mutex happend\n";
				continue;
			}
			//_tmx.lock();
			if (!_members.empty())
			{
				auto str = _members.front();
				cout << this_thread::get_id() <<" Comsumping: " << str << endl;
				_members.pop();
			}
			this_thread::sleep_for(50ms);
			_tmx.unlock();
		}
	}

	void Produce(string & s)
	{
		for (int i = 0; i < 10; ++i)
		{
			_tmx.try_lock_for(50ms);

			cout << this_thread::get_id() << " Producing : " << s << endl;

			_members.push(s);

			this_thread::sleep_for(10ms);

			_tmx.unlock();
		}
	}

	void Run()
	{
		string src{ "first day ok" };

		//std::thread(std::bind(&T_Mutex::Produce, this, src)).detach();
		//std::thread(std::bind(&T_Mutex::Comsumpe, this)).detach();

		//为啥两个 async 这里启动的线程是同一个
		//std::async(launch::async, std::bind(&T_Mutex::Comsumpe, this));
		//std::async(launch::async, std::bind(&T_Mutex::Produce, this, std::ref(src)));
		
		//对比上边，给了future返回值，async就启动了不同的线程
		auto res1 =  std::async(launch::async, std::bind(&T_Mutex::Comsumpe, this));
		auto res2 = std::async(launch::async, std::bind(&T_Mutex::Produce, this, std::ref(src)));
		system("pause");
	}

private:
	queue<string> _members;

	std::timed_mutex _tmx;
	//std::mutex _tmx;
};