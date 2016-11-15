#pragma once

#include <windows.h>
#define USR_MSG_BEGIN WM_USER+100
#define USR_MSG_END   WM_USER+1000

// 线程重用函数
class BaseThreadFun
{
public:
	virtual  void func();
};


// 线程有挂起,恢复,结束,强制结束,运行状态
// 线程需要重置运行函数,以便用于实现线程重用
// 线程要有线程唯一标识,用于识别线程.

namespace NS_ThreadState
{
	enum E_ThreadState
	{
		Creating, // 正在创建中
		Suspend,  // 暂停中
		Runing,   // 运行中
		Dead,     // 死亡
	};
}

class BaseThread
{
public:
	BaseThread();
	virtual ~BaseThread();

	virtual bool Runable();

	void CloseHandle();
	unsigned GetThreadID();

	//bool SuspendThread();

	//DWORD SuspendThread(HANDLEhThread);

	virtual unsigned fun();

protected:
	HANDLE m_hStartEvent;
	HANDLE m_hThread;
	unsigned m_nThreadID;

	NS_ThreadState::E_ThreadState m_threadState;
};

