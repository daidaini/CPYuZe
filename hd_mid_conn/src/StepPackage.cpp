#include "StepPackage.h"
#include "StringFunc.h"

#include <string.h>

PB_FrameHead StepPackage::m_PackHead{};

void StepPackage::FillHeadForRequest(const std::string& request, char* realRequest, int& reqSize)
{
	m_PackHead.sign = 69;
	m_PackHead.crypt = 0;
	m_PackHead.PackageNum = 1;
	m_PackHead.PackageNo = 0;
	m_PackHead.PackageSize = static_cast<unsigned short>(request.length() + sizeof(PB_FrameHead));
	
	::memcpy(realRequest, &m_PackHead, sizeof(PB_FrameHead));
	::memcpy(realRequest + sizeof(PB_FrameHead), request.c_str(), request.length());
	reqSize = m_PackHead.PackageSize;
}


ErrorInfo StepPackage::ParseStepRsp(const std::string& rsp)
{
	auto results = str::Split(rsp, '&');

	ErrorInfo err{};
	int endFlag = 2;
	for (const std::string& item : results)
	{
		if (endFlag == 0)
			break;
		auto id_val = str::Split(item, '=');
		if (!id_val.empty())
		{
			std::string id = id_val.at(0);
			if (id == STEP_CODE)
			{
				err.ErrCode = TransErrCode(id_val.at(1));
				--endFlag;
			}
			else if (id == STEP_MSG)
			{
				std::string& errmsg = id_val.at(1);
				strncpy(err.ErrMsg, errmsg.c_str(), sizeof(err.ErrMsg));
				--endFlag;
			}
		}
	}
	return err;
}

ErrCodeEnum StepPackage::TransErrCode(const std::string& stepErrcode)
{
	int errcode = std::stoi(stepErrcode);
	if(errcode >= 0)
		return ErrCodeEnum::Success;

	if (stepErrcode == "-1")
		return ErrCodeEnum::VerifyError;

	if (stepErrcode == "-2" || stepErrcode == "-3")
		return ErrCodeEnum::NetError;

	else
		return ErrCodeEnum::OtherError;
}