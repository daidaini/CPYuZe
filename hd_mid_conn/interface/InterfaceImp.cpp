#include "Interface.h"

#include "VerifyHandle.h"

//void OnRspLoginEx(ErrorInfo* error, int index)
//{
//	cout << "OnRspLogin : errcode = " << static_cast<int>(error->ErrCode) << "[" << error->ErrMsg << "] Index = " << index << endl;
//}

bool Init(CallbackFunc callback)
{
	VerifyHandle::GetInstance().SetCallback(callback);
	return VerifyHandle::GetInstance().Init();
}

void Verify(const char* request, int reqlen, int index)
{
	try
	{
		VerifyHandle::GetInstance().AddRequest(index, std::string(request, reqlen));
	}
	catch (const std::exception& e)
	{
		Logger::GetInstance().Warn("Some exception happeded({})", e.what());
	}
}

