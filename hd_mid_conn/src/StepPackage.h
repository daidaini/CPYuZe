#pragma once

#pragma pack(1)
struct PB_FrameHead
{
	//协议标识，目前定义为66
	unsigned char sign;
	//0表示未加密，1表示数据加密了
	unsigned char crypt : 2;
	//未使用
	unsigned char unused : 6;
	//通讯包总数
	unsigned char PackageNum;		
	//当前通讯包序号，从0开始，当为PackageNum-1时表示是最后一个通讯包
	unsigned char PackageNo;		
	//校验码（对通讯包体进行CRC16校验，在加密压缩后）
	unsigned short CheckCode;		
	//包体的长度，加密压缩后
	unsigned short PackageSize;
} ;
#pragma pack()

//错误代码
#define STEP_CODE "1"
//错误信息
#define STEP_MSG  "2"

#include <string>
#include "Interface.h"

class StepPackage final
{
public:
	StepPackage() = default;
	~StepPackage() = default;

	static void FillHeadForRequest(const std::string& request, char* realRequest, int& reqSize);

	static ErrorInfo ParseStepRsp(const std::string& rsp);
private:
	static PB_FrameHead m_PackHead;

	static ErrCodeEnum TransErrCode(const std::string& stepErrcode);

};