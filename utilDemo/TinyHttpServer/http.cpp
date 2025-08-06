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
#define strcasecmp _stricmp
#elif defined(__linux__)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include <sys/wait.h>
#endif

#define IsSpace(x) isspace((int)(x))
#define SERVER_STRING "Server: tinyhttpserver/0.1.0\r\n"
#define STDIN   0
#define STDOUT  1
#define STDERR  2

typedef unsigned short u_short;
typedef int			   socklen_t;

typedef struct {
	char data[8192];
	int pos = 0;
	int len = 0;
}LineBuffer;

//����http������
int startup(u_short* port);
//��ӡ������Ϣ���˳�
void error_die(const char *msg);
//��������֧������Ĺ��ܣ��޷��������
void unimplemented(int);
//��http�������л�ȡһ��
int get_line(int sockfd, char * line, int size, LineBuffer& buffer);
//�̺߳����� ����http����
void accept_request(void *arg);
//����httpͷ
void discard_headers(int sockfd, LineBuffer& buffer);
//û���ҵ�����Դ ������Ϣ
void not_found(int client);
//����������
void cannot_execute(int client);
//����ȷ������
void bad_request(int client);
//���ط������ļ�
void serve_file(int sockfd, const char *filename);
//ִ��cgi
void execute_cgi(int sockfd, const char *filename, const char *method, const char *querystring, LineBuffer& buffer);
//������Ϣͷ
void headers(int sockfd, const char *filename);
//�����ļ�����
void cat(int sockfd, FILE *pFile);

void discard_headers(int sockfd, LineBuffer& buffer)
{
	char line[1024];
	int ret = get_line(sockfd, line, sizeof(line), buffer);
	while (ret > 0 && strcmp(line, "\n"))
	{
		ret = get_line(sockfd, line, sizeof(line), buffer);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)  // ������ name , port
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

	//���õ�ַ����
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

	//��������portΪ0���������˿�
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

int get_line(int sockfd, char * line, int size, LineBuffer& buffer)
{
	if (size <= 0)
	{
		return -1;
	}

	//����\r\nΪһ��
	int n = 0;
	while (n < size - 1)
	{
		if (buffer.pos >= buffer.len)
		{
			int ret = recv(sockfd, buffer.data, sizeof(buffer.data), 0);
			if (ret <= 0)
			{
				break;
			}
			buffer.pos = 0;
		}

		char c = buffer.data[buffer.pos++];
		line[n++] = c;

		if (c == '\n')
		{
			break;
		}
		if (c == '\r')
		{
			if (buffer.pos < buffer.len  && buffer.data[buffer.pos] == '\n')
			{
				if (n < size - 1)
				{
					line[n++] = '\n';
					buffer.pos++;
				}
				break;
			}
		}
	}
	line[n] = '\0';
	return n;
}

void unimplemented(int connfd)
{
	char buf[1024];
	//״̬��
	sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
	send(connfd, buf, strlen(buf), 0);
	//��Ӧͷ
	sprintf(buf, SERVER_STRING);
	send(connfd, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(connfd, buf, strlen(buf), 0);
	//����
	sprintf(buf, "\r\n");
	send(connfd, buf, strlen(buf), 0);
	//��Ӧ��
	sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
	send(connfd, buf, strlen(buf), 0);
	sprintf(buf, "</TITLE></HEAD>\r\n");
	send(connfd, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
	send(connfd, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(connfd, buf, strlen(buf), 0);
}

void not_found(int client)
{
	char buf[1024];

	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void cannot_execute(int client)
{
	char buf[1024];
	sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
	send(client, buf, strlen(buf), 0);

}

void bad_request(int client) 
{
	char buf[1024];
	sprintf(buf, "HTTP/1.0 400 Bad Request\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<P> your browser sent a bad request, ");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "such as a POST without a Content-Length.\r\n");
	send(client, buf, strlen(buf), 0);
}

void accept_request(void *arg)
{
	int connfd = *(int*)arg;
	int ret;
	LineBuffer buf;
	char line[1024];
	char method[64];
	char url[128];
	char path[512];
	int i = 0, j = 0;
	int cgi = 0;  
	char *query_string = NULL;

	struct stat st;

	//1.��ȡ������
	ret = get_line(connfd, line, sizeof(line), buf);

	//1.1 �������󷽷�

	while (!IsSpace(line[i]) && i < sizeof(method) - 1)
	{
		method[i] = line[i];
		i++;
	}
	method[i] = '\0';

	//�������GET��POST֮�����������
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
	{
		unimplemented(connfd);
		return;
	}

	//�����POST����Ҫִ��cgi
	if (strcasecmp(method, "POST"))
	{
		cgi = 1;
	}

	j = i;
	i = 0;
	//��ȡurl  ��������ѯ�ַ���(Get���󴫲�)
	while (IsSpace(line[j]) && j < ret) j++;

	while (!IsSpace(line[j]) && i < sizeof(url)-1 && j < ret)
	{
		url[i++] = line[j++];
	}
	url[i] = '\0';

	if (strcasecmp(method, "GET") == 0)
	{
		query_string = url;
		while ((*query_string != '\0') && (*query_string != '?'))
		{
			query_string++;
		}
		if (*query_string == '?')
		{
			cgi = 1;
			*query_string = '\0';
			query_string++; //��cgi�ļ��ַ���ͷ
		}
	}

	sprintf_s(path, 128, "htdocs%s", url);
	if (path[strlen(path)-1] == '/')
	{
		strcat(path, "index.html");
	}
	//�Ҳ��������Դ
	if (stat(path, &st) == -1)
	{
		discard_headers(connfd, buf);
		not_found(connfd);
	}
	else {
		if ((st.st_mode & S_IFMT) == S_IFDIR )
		{
			strcat(path, "index.html");
		}

#if defined(_WIN32)
		if (st.st_mode & S_IEXEC)
#elif defined(__linux__)
		if ((st.st_mode & S_IXUSR) ||
			(st.st_mode & S_IXGRP) ||
			(st.st_mode & S_IXOTH))
#endif
		{
			cgi = 1;
		}

		if (cgi)
		{
			execute_cgi(connfd, path, method, query_string);
		}else {
			serve_file(connfd, path);
		}
	}

	//����ͷ
	//����
	//������
#if defined(_WIN32)
	closesocket(connfd);
#elif defined(__linux__)
	close(connfd);
#endif
}

void headers(int sockfd, const char *filename)
{
	char buf[1024];
	//״̬��
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(sockfd, buf, strlen(buf), 0);
	//��Ӧͷ
	strcpy(buf, SERVER_STRING);
	send(sockfd, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(sockfd, buf, strlen(buf), 0);
	//����
	strcpy(buf, "\r\n");
	send(sockfd, buf, strlen(buf), 0);
}

void cat(int sockfd, FILE *pFile)
{
	char buf[1024];

	fgets(buf, sizeof(buf), pFile);
	while (!feof(pFile))
	{
		send(sockfd, buf, strlen(buf), 0);
		fgets(buf, sizeof(buf), pFile);
	}
}

void serve_file(int sockfd, const char *filename, LineBuffer& buffer)
{
	//open file
	FILE *resource = NULL;
	int num = 1;
	char buf[1024];

	buf[0] = 'A'; buf[1] = '\0';
	while (num > 0 && strcmp("\n", buf))
		num = get_line(sockfd, buf, sizeof(buf), buffer);
	//read file
	resource = fopen(filename, "r");
	if (resource == NULL)
	{
		not_found(sockfd);
	}
	else {
		//write msg to client
		headers(sockfd, filename);
		//��Ӧ��
		cat(sockfd, resource);
	}
	//close file
	fclose(resource);
}

void execute_cgi(int sockfd, const char *filename, const char *method, const char *querystring, LineBuffer& buffer)
{
	int cgi_input[2];
	int cgi_output[2];
	char buf[1024];
	int content_length = -1;
	int status;

	if (strcasecmp(method, "GET") == 0)
	{
		discard_headers(sockfd, buffer);
	}
	else if (strcasecmp(method, "POST") == 0)
	{
		int num = get_line(sockfd, buf, sizeof(buf), buffer);
		while (num > 0 && strcmp("\n", buf))
		{
			buf[15] = '\0';
			if (strcasecmp(buf, "Content-Length:") == 0)
			{
				content_length = atoi(&buf[16]);
			}
			num = get_line(sockfd, buf, sizeof(buf), buffer);
		}
		if (content_length == -1)
		{
			bad_request(sockfd);
			return;
		}
	}
	else {
		//
	}

	int ret = pipe(cgi_input);
	if (ret < 0)
	{
		cannot_execute(sockfd);
		return;
	}
	ret = pipe(cgi_output);
	if (ret < 0)
	{
		cannot_execute(sockfd);
		return;
	}

	//�ӽ���ִ��cgi��������� �����̶�ȡ������֯��html�����ͻ���
	pid_t pid = fork();
	if (pid < 0)
	{
		cannot_execute(sockfd);
		return;
	}

	//״̬��
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	send(sockfd, buf, strlen(buf), 0);

	if (pid == 0)//�ӽ���ִ��cgi�������̿��Է���Ϣ���ӽ��̣��ӽ���ͨ��STDIN��ȡ
	{
		char methodstr[255];
		char querystr[255];
		char lengthstr[255];

		dup2(cgi_output[1], STDOUT);
		dup2(cgi_input[0], STDIN);
		close(cgi_output[0]);
		close(cgi_input[1]);

		sprintf(methodstr, "REQUEST_METHOD=%s", method);
		putenv(methodstr);

		if (strcasecmp(method, "GET")==0)
		{
			sprintf(querystr, "QUERY_STRING=%s", querystring);
			putenv(querystr);
		}
		else {
			sprintf(lengthstr, "CONTENT_LENGTH=%d", content_length);
			putenv(lengthstr);
		}
		execl(filename, NULL);
		exit(0);
	}
	else {
		//�����̶�ȡ�ӽ��̷��صĽ�������͸��ͻ���
		close(cgi_output[1]);
		close(cgi_input[0]);
		char c;
		if (strcasecmp(method, "POST")==0)
		{
			for (int i = 0; i < content_length;i++)
			{
				recv(sockfd, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		}

		while (read(cgi_output[0], &c, 1)>0)
		{
			send(sockfd, &c, 1, 0);
		}

		close(cgi_output[0]);
		close(cgi_input[1]);
		waitpid(pid, &status, 0);
	}
}