
#pragma once

enum ErrCodeEnum
{
	//�ɹ�
	Success,
	//��֤ʧ��
	VerifyError,
	//�������
	NetError,
	//��������
	OtherError
};

struct ErrorInfo
{
	ErrCodeEnum ErrCode;
	char ErrMsg[256];
};

#ifdef WIN32
#define HD_MID_API extern "C" __declspec(dllexport)
#else
#define HD_MID_API
#endif // Win32


/*�ص���������
* ErrorInfo Ϊ���ش�����Ϣ
*	Success ��ʾ�ɹ�
*	VerifyError ��ʾ��̨У��ʧ��
*	NetError ��ʾ���������Ҫ�����ط�
*	OtherError �������󣬿��Ժ���
* 
* index Ϊ��Ӧ�����е����
*/
typedef void (*CallbackFunc)(ErrorInfo* error, int index);


/*��ʼ��������ֻ�����һ��
* �ڲ������ˣ���ȡ������Ϣ���Լ����ûص�����
* callback �Ƕ���Ļص�����
* ����true Ϊ�ɹ���falseΪʧ��
*/
HD_MID_API bool Init(CallbackFunc callback);

/*���ڷ�����֤����
* request ��Ӧ������
* reqlen  ��Ӧ�����ĳ���
* index	�Զ����������ţ���������������Ҫ���ظ�����ص����ص�����һһ��Ӧ��
*/
HD_MID_API void Verify(const char* request, int reqlen, int index);

