#pragma once

#include <stdio.h>
#include <string.h>


#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <mutex>
#include <vector>
#include <queue>
#include <forward_list>
#include <memory>
#include <string>
#include <utility>
#include <map>
#include <unordered_map>
#include <assert.h>
#include <deque>
using namespace std;

namespace self_test
{
	class noncopyable
	{
		noncopyable(const noncopyable&) = delete;
		void operator=(const noncopyable&) = delete;

	protected:
		noncopyable() = default;
		~noncopyable() = default;
	};
}
