
#include "baseThread.h"
#include <process.h>
#include <cstdio>

extern unsigned gMainThreadID;

// thread callback function
namespace NS_ThreadFun
{
	unsigned __stdcall fun(void *param)
	{
		BaseThread * baseThread = (BaseThread*)param;
		baseThread->fun();
		return 0;
	}
}


BaseThread::BaseThread()
:m_hStartEvent(NULL),
 m_hThread(NULL),
 m_nThreadID(-1)
{
}

BaseThread::~BaseThread()
{
}

bool BaseThread::Runable()
{
	bool result = false;

	// 创建互斥事件
	m_hStartEvent = ::CreateEvent(0, FALSE, FALSE, 0); //create thread start event
	if (m_hStartEvent == NULL)
	{
		printf("create start event failed,errno:%d\n", ::GetLastError());
		return false;
	}

	// 创建线程
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &NS_ThreadFun::fun, this, 0, &m_nThreadID);
	
	if (m_hThread == NULL)
	{
		printf("start thread failed,errno:%d\n", ::GetLastError());
		CloseHandle();
		return false;
	}

	//wait thread start event to avoid PostThreadMessage return errno:1444
	::WaitForSingleObject(m_hStartEvent, INFINITE);
	CloseHandle();
	return true;
}

void BaseThread::CloseHandle()
{
	if (m_hStartEvent)
	{
		::CloseHandle(m_hStartEvent);
		m_hStartEvent = NULL;
	}

}

unsigned BaseThread::GetThreadID()
{
	return m_nThreadID;
}

unsigned BaseThread::fun()
{
	printf("thread fun start\n");

	MSG msg;
	PeekMessage(&msg, NULL, USR_MSG_BEGIN, USR_MSG_BEGIN, PM_NOREMOVE);

	if (!SetEvent(m_hStartEvent)) //set thread start event 
	{
		printf("set start event failed,errno:%d\n", ::GetLastError());
		return 1;
	}


	while (true)
	{
		if (PeekMessage(&msg, NULL, USR_MSG_BEGIN, USR_MSG_END, PM_REMOVE)) //get msg from message queue
		{
			switch (msg.message)
			{
			case USR_MSG_BEGIN + 1:
			{
				char * pInfo = (char *)msg.wParam;
				printf("threadId::%d, recv %s\n", m_nThreadID, pInfo);
				delete[] pInfo;

				if (!PostThreadMessage(gMainThreadID, USR_MSG_BEGIN+10, m_nThreadID, 0))//post thread msg											  //if (!PostThreadMessage(myThread->GetThreadID(), MY_MSG, myThread->GetThreadID(), 0))//post thread msg
				{
					printf("send message to main thread failed,errno:%d\n", ::GetLastError());
				}

				break;
			}
			default:
				printf("1111111111111\n");
			}
		}
		else
		{
			//printf("kong zhuan\n");
		}
	};
	return 0;
}


void BaseThreadFun::func()
{

}
