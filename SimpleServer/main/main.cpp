#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include<iostream>
#include<cassert>
#pragma comment (lib,"ws2_32.lib")
#include<ws2tcpip.h>
#define LISTEN_PORT 99998
#define LIATEN_BACKLOG 32
using namespace std;
/*********************************************************************************
*                   ��������
**********************************************************************************/
//accept�ص�����
void do_accept_cb(evutil_socket_t listener, short event, void *arg);
//read �ص�����
void read_cb(struct bufferevent *bev, void *arg);
//error�ص�����
void error_cb(struct bufferevent *bev, short event, void *arg);
//write �ص�����
void write_cb(struct bufferevent *bev, void *arg);
/*********************************************************************************
*                   ������
**********************************************************************************/
//accept�ص�����
void do_accept_cb(evutil_socket_t listener, short event, void *arg)
{
	//�����event_baseָ��
	struct event_base *base = (struct event_base*)arg;
	//socket������
	evutil_socket_t fd;
	//������ַ
	struct sockaddr_in sin;
	//��ַ��������
	socklen_t slen = sizeof(sin);
	//���տͻ���
	fd = accept(listener, (struct sockaddr *)&sin, &slen);
	if (fd < 0)
	{
		perror("error accept");
		return;
	}
	printf("ACCEPT: fd = %u\n", fd);
	////ע��һ��bufferevent_socket_new�¼�
	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	////���ûص�����
	bufferevent_setcb(bev, read_cb, NULL, error_cb, arg);
	////���ø��¼�������
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}
////read �ص�����
void read_cb(struct bufferevent *bev, void *arg)
{
#define MAX_LINE 256
	char line[MAX_LINE + 1];
	int n;
	//ͨ���������bev�ҵ�socket fd
	evutil_socket_t fd = bufferevent_getfd(bev);
	//
	while (n = bufferevent_read(bev, line, MAX_LINE))
	{
		line[n] = '\0';
		printf("fd=%u, read line: %s\n", fd, line);
		//����ȡ�����ݷ��ظ��ͻ���
		bufferevent_write(bev, line, n);
	}
}
////error�ص�����
void error_cb(struct bufferevent *bev, short event, void *arg)
{
	//ͨ���������bev�ҵ�socket fd
	evutil_socket_t fd = bufferevent_getfd(bev);
	//cout << "fd = " << fd << endl;
	if (event & BEV_EVENT_TIMEOUT)
	{
		printf("Timed out\n"); //if bufferevent_set_timeouts() called
	}
	else if (event & BEV_EVENT_EOF)
	{
		printf("connection closed\n");
	}
	else if (event & BEV_EVENT_ERROR)
	{
		printf("some other error\n");
	}
	bufferevent_free(bev);
}
////write �ص�����
void write_cb(struct bufferevent *bev, void *arg)
{
	char str[50];
	//ͨ���������bev�ҵ�socket fd
	evutil_socket_t fd = bufferevent_getfd(bev);
	//cin >> str;
	printf("�������ݣ�");
	scanf_s("%d", &str);
	bufferevent_write(bev, &str, sizeof(str));
}

int main()
{
	int ret;
	evutil_socket_t listener;
	WSADATA Ws;
	//Init Windows Socket
	if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
	{
		return -1;
	}
	listener = socket(AF_INET, SOCK_STREAM, 0);
	assert(listener > 0);
	evutil_make_listen_socket_reuseable(listener);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(LISTEN_PORT);
	if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("bind");
		return 1;
	}
	if (listen(listener, 1000) < 0) {
		perror("listen");
		return 1;
	}
	printf("Listening...\n");
	evutil_make_socket_nonblocking(listener);
	struct event_base *base = event_base_new();
	assert(base != NULL);
	struct event *listen_event;
	listen_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept_cb, (void*)base);
	event_add(listen_event, NULL);
	event_base_dispatch(base);
	printf("The End.");
	return 0;
}