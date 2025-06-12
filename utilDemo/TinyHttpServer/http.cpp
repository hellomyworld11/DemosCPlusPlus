//#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <thread>

#if defined(_WIN32)
#include <windows.h>
#include <WinSock2.h> 
#elif defined(__linux__)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include <sys/wait.h>
#endif

typedef unsigned short u_short;
typedef int			   socklen_t;

int startup(u_short* port)
{
#if defined(_WIN32)
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
	{
		error_die("WSAStartup");
	}
#endif

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		error_die("socket");

	//设置地址重用
	bool bOpt = true;
	int lenOpt = sizeof(bool);
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt, lenOpt);
	if (ret < 0)
	{
		error_die("setsockopt");
	}

	sockaddr_in server_info;
	server_info.sin_port = htons(*port);
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = htonl(ADDR_ANY);
	socklen_t server_info_len = sizeof(server_info);
	ret = bind(sockfd, (sockaddr*)&server_info, server_info_len);
	if (ret < 0)
	{
		error_die("bind");
	}

	//如果传入的port为0则随机分配端口
	if (*port == 0)
	{
		if (getsockname(sockfd, (sockaddr*)&server_info, &server_info_len) < 0)
			error_die("getsockname");

		*port = ntohs(server_info.sin_port);
	}

	if (listen(sockfd, 5) < 0)
		error_die("listen");

	return sockfd;
}

void error_die(const char *msg)
{
	perror(msg);
	exit(1);
}

int get_line(int sockfd, char * buf, int size)
{
	do {
		recv(sockfd, )


	} while (1)
	
}

void accept_request(void *arg)
{
	int connfd = *(int*)arg;

	//读取请求行



	//请求头

	//空行

	//请求体


}

int main(int argc, char *argv[])
{
	if (argc != 2)  // name , port
	{
		return 0;
	}

	char *portstr = argv[1];
	u_short port = atoi(portstr);
	
	int sockfd = startup(&port);
	int clientfd = -1;
	sockaddr_in client_info;
	socklen_t client_info_len = sizeof(client_info);

	while (1)
	{
		int connfd = accept(sockfd, (sockaddr*)&client_info, &client_info_len);
		
		if (connfd == -1)
			error_die("accept");

		try
		{
			std::thread t(accept_request, connfd);
			t.detach();
		}
		catch (const std::system_error& e)
		{
			error_die("pthread_create");
		}
	}

#if defined(_WIN32)
	closesocket(sockfd);
#elif defined(__linux__)
	close(sockfd);
#endif
	
	getchar();
	return 0;
}