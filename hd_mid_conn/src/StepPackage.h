#pragma once

#pragma pack(1)
struct PB_FrameHead
{
	//Э���ʶ��Ŀǰ����Ϊ66
	unsigned char sign;
	//0��ʾδ���ܣ�1��ʾ���ݼ�����
	unsigned char crypt : 2;
	//δʹ��
	unsigned char unused : 6;
	//ͨѶ������
	unsigned char PackageNum;		
	//��ǰͨѶ����ţ���0��ʼ����ΪPackageNum-1ʱ��ʾ�����һ��ͨѶ��
	unsigned char PackageNo;		
	//У���루��ͨѶ�������CRC16У�飬�ڼ���ѹ����
	unsigned short CheckCode;		
	//����ĳ��ȣ�����ѹ����
	unsigned short PackageSize;
} ;
#pragma pack()

//�������
#define STEP_CODE "1"
//������Ϣ
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