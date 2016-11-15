#pragma once

#include <windows.h>
#define USR_MSG_BEGIN WM_USER+100
#define USR_MSG_END   WM_USER+1000

// �߳����ú���
class BaseThreadFun
{
public:
	virtual  void func();
};


// �߳��й���,�ָ�,����,ǿ�ƽ���,����״̬
// �߳���Ҫ�������к���,�Ա�����ʵ���߳�����
// �߳�Ҫ���߳�Ψһ��ʶ,����ʶ���߳�.

namespace NS_ThreadState
{
	enum E_ThreadState
	{
		Creating, // ���ڴ�����
		Suspend,  // ��ͣ��
		Runing,   // ������
		Dead,     // ����
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

