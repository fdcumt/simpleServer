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
返回值如果成功，则返回非零值；否则返回0。
参数message用户自定义消息的ID。
wParam第一个消息参数。lParam第二个消息参数。
说明调用这个函数以向其它CWinThread对象发送一个用户自定义消息。发送的消息通过消息映射宏ON_THREAD_MESSAGE被映射到适当的消息处理函数。

注意事项消息将寄送到的线程必须创建消息队列，否则调用PostThreadMessage会失败。用下列方法之一来处理这种情况：
	1.调用PostThreadMessage。如果失败，调用Sleep，再调用PostThreadMessage，反复执行，直到PostThreadMessage成功。
	2.创建一个事件对象，再创建线程。在调用PostThreadMessage之前，用函数WaitForSingleObject来等待事件被设置为被告知状态。
	消息将寄送到的线程调用PeekMessage（&msg，NULL，WM_USER，WM_USER，PM_NOREMOVE）来强制系统创建消息队列。
	设置事件，表示线程已准备好接收寄送的消息。消息将寄送到的线程通过调用GetMesssge或PeekMesssge来取得消息。返回的MSG结构中的hwnd成员为NULL。
	  
每一个消息队列将队列内的消息限制在10,000个。这个限制应该已经足够的大。
如果一个程序超过这个限制，它应当被重新设计以避免占用如此多的系统资源。
要修改消息个数的限制，应当修改注册表中对应的项。
*/