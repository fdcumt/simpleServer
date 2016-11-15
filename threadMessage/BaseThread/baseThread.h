#pragma once

#include <windows.h>
#define USR_MSG_BEGIN WM_USER+100
#define USR_MSG_END   WM_USER+1000

class BaseThread
{
public:
	BaseThread();
	virtual ~BaseThread();

	virtual bool Runable();

	void CloseHandle();
	unsigned GetThreadID();

	virtual unsigned fun();

protected:
	HANDLE m_hStartEvent;
	HANDLE m_hThread;
	unsigned m_nThreadID;
	bool m_bRunning;
};

