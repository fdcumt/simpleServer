#include <windows.h>
#include <cstdio>
#include <process.h>

#include "BaseThread/baseThread.h"

unsigned gMainThreadID = -1;

#define MY_MSG USR_MSG_BEGIN+1
const int MAX_INFO_SIZE = 20;



void sendMsg(BaseThread* myThread)
{
	char* pInfo = new char[MAX_INFO_SIZE]; //create dynamic msg
	sprintf_s(pInfo, MAX_INFO_SIZE - 1, "msg_%d", myThread->GetThreadID());
	printf("post message pInfo:%s\n", pInfo);
	if (!PostThreadMessage(myThread->GetThreadID(), MY_MSG, (WPARAM)pInfo, 0))//post thread msg
	//if (!PostThreadMessage(myThread->GetThreadID(), MY_MSG, myThread->GetThreadID(), 0))//post thread msg
	{
		printf("post message failed,errno:%d\n", ::GetLastError());
		delete[] pInfo;
	}

}

void replyMsg(MSG msg, BaseThread* myThread)
{
	if (msg.wParam == myThread->GetThreadID())
	{
		sendMsg(myThread);
	}
}

int main()
{

	gMainThreadID = (unsigned)GetCurrentThreadId();

	BaseThread baseThread1;
	BaseThread baseThread2;
	BaseThread baseThread3;
	BaseThread baseThread4;
	baseThread1.Runable();
	baseThread2.Runable();
	baseThread3.Runable();
	baseThread4.Runable();

	sendMsg(&baseThread1);
	sendMsg(&baseThread2);
	sendMsg(&baseThread3);
	sendMsg(&baseThread4);

	::Sleep(1000);

	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, NULL, USR_MSG_BEGIN, USR_MSG_END, PM_REMOVE)) //get msg from message queue
		{
			switch (msg.message)
			{
			case USR_MSG_BEGIN + 10:
			{
				replyMsg(msg, &baseThread1);
				replyMsg(msg, &baseThread2);
				replyMsg(msg, &baseThread3);
				replyMsg(msg, &baseThread4);
				break;
			}
			default:
				printf("1111111111111\n");
			}
		}
		system("pause");


	}

	return 0;
}
/*
BOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);
����ֵ����ɹ����򷵻ط���ֵ�����򷵻�0��
����message�û��Զ�����Ϣ��ID��
wParam��һ����Ϣ������lParam�ڶ�����Ϣ������
˵���������������������CWinThread������һ���û��Զ�����Ϣ�����͵���Ϣͨ����Ϣӳ���ON_THREAD_MESSAGE��ӳ�䵽�ʵ�����Ϣ��������

ע��������Ϣ�����͵����̱߳��봴����Ϣ���У��������PostThreadMessage��ʧ�ܡ������з���֮һ���������������
	1.����PostThreadMessage�����ʧ�ܣ�����Sleep���ٵ���PostThreadMessage������ִ�У�ֱ��PostThreadMessage�ɹ���
	2.����һ���¼������ٴ����̡߳��ڵ���PostThreadMessage֮ǰ���ú���WaitForSingleObject���ȴ��¼�������Ϊ����֪״̬��
	��Ϣ�����͵����̵߳���PeekMessage��&msg��NULL��WM_USER��WM_USER��PM_NOREMOVE����ǿ��ϵͳ������Ϣ���С�
	�����¼�����ʾ�߳���׼���ý��ռ��͵���Ϣ����Ϣ�����͵����߳�ͨ������GetMesssge��PeekMesssge��ȡ����Ϣ�����ص�MSG�ṹ�е�hwnd��ԱΪNULL��
	  
ÿһ����Ϣ���н������ڵ���Ϣ������10,000�����������Ӧ���Ѿ��㹻�Ĵ�
���һ�����򳬹�������ƣ���Ӧ������������Ա���ռ����˶��ϵͳ��Դ��
Ҫ�޸���Ϣ���������ƣ�Ӧ���޸�ע����ж�Ӧ���
*/