#include <windows.h>
#include <cstdio>
#include <process.h>

#define MY_MSG WM_USER+100
const int MAX_INFO_SIZE = 20;

HANDLE hStartEvent; // thread start event

					// thread function
unsigned __stdcall fun(void *param)
{
	printf("thread fun start\n");

	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	if (!SetEvent(hStartEvent)) //set thread start event 
	{
		printf("set start event failed,errno:%d\n", ::GetLastError());
		return 1;
	}

	while (true)
	{
		if (PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE)) //get msg from message queue
		{
			switch (msg.message)
			{
			case MY_MSG:
			{
				char * pInfo = (char *)msg.wParam;
				printf("recv %s\n", pInfo);
				delete[] pInfo;
				break;
			}
			default:
				printf("kong zhuan\n");
			}
		}
		else 
		{
			//printf("kong zhuan\n");
		}
	};
	return 0;
}

int main()
{
	HANDLE hThread;
	unsigned nThreadID;

	hStartEvent = ::CreateEvent(0, FALSE, FALSE, 0); //create thread start event
	if (hStartEvent == 0)
	{
		printf("create start event failed,errno:%d\n", ::GetLastError());
		return 1;
	} 

	//start thread
	hThread = (HANDLE)_beginthreadex(NULL, 0, &fun, NULL, 0, &nThreadID);
	if (hThread == 0)
	{
		printf("start thread failed,errno:%d\n", ::GetLastError());
		CloseHandle(hStartEvent);
		return 1;
	}

	//wait thread start event to avoid PostThreadMessage return errno:1444
	::WaitForSingleObject(hStartEvent, INFINITE);
	CloseHandle(hStartEvent);

	int count = 0;
	while (true)
	{
		char* pInfo = new char[MAX_INFO_SIZE]; //create dynamic msg
		sprintf_s(pInfo, MAX_INFO_SIZE-1, "msg_%d", ++count);
		if (!PostThreadMessage(nThreadID, MY_MSG, (WPARAM)pInfo, 0))//post thread msg
		{
			printf("post message failed,errno:%d\n", ::GetLastError());
			delete[] pInfo;
		}
		::Sleep(1000);
	}

	CloseHandle(hThread);
	return 0;
}
/*
CWinThread::PostThreadMessageBOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAMlParam);
����ֵ����ɹ����򷵻ط���ֵ�����򷵻�0��
����message�û��Զ�����Ϣ��ID��
wParam��һ����Ϣ������lParam�ڶ�����Ϣ������
˵���������������������CWinThread������һ���û��Զ�����Ϣ�����͵���Ϣͨ����Ϣӳ���ON_THREAD_MESSAGE��ӳ�䵽�ʵ�����Ϣ��������

ע��������Ϣ�����͵����̱߳��봴����Ϣ���У��������PostThreadMessage��ʧ�ܡ������з���֮һ���������������
	1.����PostThreadMessage�����ʧ�ܣ�����Sleep���ٵ���PostThreadMessage������ִ�У�ֱ��PostThreadMessage�ɹ���
	2.����һ���¼������ٴ����̡߳��ڵ���PostThreadMessage֮ǰ���ú���WaitForSingleObject���ȴ��¼�������Ϊ����֪״̬��
	��Ϣ�����͵����̵߳���PeekMessage��&msg��NULL��WM_USER��WM_USER��PM_NOREMOVE����ǿ��ϵͳ������Ϣ���С�
	�����¼�����ʾ�߳���׼���ý��ռ��͵���Ϣ����Ϣ�����͵����߳�ͨ������GetMesssge��PeekMesssge��ȡ����Ϣ�����ص�MSG�ṹ�е�hwnd��ԱΪNULL��
	  
ÿһ����Ϣ���н������ڵ���Ϣ������10, 000�����������Ӧ���Ѿ��㹻�Ĵ�
���һ�����򳬹�������ƣ���Ӧ������������Ա���ռ����˶��ϵͳ��Դ��
Ҫ�޸���Ϣ���������ƣ�Ӧ���޸�ע����ж�Ӧ���
*/