
#pragma once

enum ErrCodeEnum
{
	//成功
	Success,
	//验证失败
	VerifyError,
	//网络错误
	NetError,
	//其他错误
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


/*回调函数声明
* ErrorInfo 为返回错误信息
*	Success 表示成功
*	VerifyError 表示后台校验失败
*	NetError 表示网络错误，需要尝试重发
*	OtherError 其他错误，可以忽略
* 
* index 为对应请求中的序号
*/
typedef void (*CallbackFunc)(ErrorInfo* error, int index);


/*初始化操作，只需调用一次
* 内部包括了，读取配置信息，以及设置回调函数
* callback 是定义的回调函数
* 返回true 为成功，false为失败
*/
HD_MID_API bool Init(CallbackFunc callback);

/*用于发送验证请求
* request 对应请求报文
* reqlen  对应请求报文长度
* index	自定义的索引序号，可以自增处理，需要不重复（与回调返回的索引一一对应）
*/
HD_MID_API void Verify(const char* request, int reqlen, int index);

