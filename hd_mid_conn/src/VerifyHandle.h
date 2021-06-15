#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

#include "ConnectionPool.h"
#include "TaskHandle.h"
#include "Interface.h"

using namespace std;
class VerifyHandle
{
	using CallbackType = function<void(ErrorInfo*, int)>;
public:
	static VerifyHandle& GetInstance();

	bool Init();
	void SetCallback(CallbackType func);

	void AddRequest(int index, const string& request);

	void OnResult(ErrCodeEnum errcode, const string& errmsg, int index);

private:
	VerifyHandle();
	~VerifyHandle();

	void HandleRequestTask(int index, const string request);

private:
	mutex m_Mt;
	condition_variable m_Cv;

	unique_ptr<TaskHandle> m_TaskHandle;

	CallbackType m_RspFunc;
};
