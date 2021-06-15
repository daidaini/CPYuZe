#include "VerifyHandle.h"


void OnRspLogin(ErrorInfo* error, int index)
{
	cout << "OnRspLogin : errcode = " << static_cast<int>(error->ErrCode) <<"[" << error->ErrMsg <<"] Index = " << index << endl;
}

string login_request_1 = "58=zkmb8iFZcB0\a&11=0&3=6011&2061=0&6=1&7=1&\n"
"249=17368674639&67=6&53=0&62=123&70=12365544&72=9U99SEF4GBFG&71=127.0.0.1&349=&350=&351=BFEBFBFF000306F2\\nBFEBFBFF000306F2"
"&3=12&281=18516275549&73=HDV6.2.3.4-2.2.6.6.20200312&255=1&254=1&445=AXZQ2020&485=ypfq,sss,123&7034=DPanxtjbh&";

string login_request = "58=M5fg6uKmILE\a&11=0&3=6011&2061=0&6=1&7=1&\n"
"249=101302822&67=6&53=0&62=123&70=12365544&72=9U99SEF4GBFG&71=127.0.0.1&349=&350=&351=BFEBFBFF000306F2\\nBFEBFBFF000306F2"
"&3=12&281=18516275549&73=HDV6.2.3.4-2.2.6.6.20200312&255=1&254=1&445=AXZQ2020&485=ypfq,sss,123&7034=DPanxtjbh&";

int main()
{
	//system("chcp 65001");

	VerifyHandle::GetInstance().Init();

	VerifyHandle::GetInstance().SetCallback(OnRspLogin);

	int index = 0;
	for (int i = 0; i < 6; ++i)
	{
		VerifyHandle::GetInstance().AddRequest(++index, login_request);
		VerifyHandle::GetInstance().AddRequest(++index, login_request);
		VerifyHandle::GetInstance().AddRequest(++index, login_request);
		VerifyHandle::GetInstance().AddRequest(++index, login_request);
		VerifyHandle::GetInstance().AddRequest(++index, login_request);

		this_thread::sleep_for(chrono::seconds(6));
	}
	

	while (true)
	{
		this_thread::sleep_for(chrono::seconds(1));

	}

	system("pause");
	return 0;
}