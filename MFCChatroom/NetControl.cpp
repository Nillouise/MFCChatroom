#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<NetCon.h>
#include <winsock2.h>  
#include <windows.h>  
#include <stdio.h>  
#include<vector>
#include<string>
#include<sstream>
#pragma comment(lib, "ws2_32.lib")  

using namespace std;

string remoteServerIP;
int remoteServerPort = 0;
int remoteServerIPflag = 0;
sockaddr_in remoteAddr;

int localServerPort = 0;
int localMachineIsServerflag = 0;
vector<string> clientIpList;
vector<int> clientPortList;

SOCKET sendSocket;
SOCKET recvSocket;


string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}

int stringToInt(string s)
{
	stringstream ss;
	ss << s;
	int i;
	ss >> i;
	return i;
}

//设置非阻塞
void setnonblocking(int sockfd) 
{
	unsigned long ul = 1;//0：阻塞
	int ret;
	ret = ioctlsocket(sockfd, FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式。
}
int SendMsgToServer(CString s)
{
	if (remoteServerIPflag == 0)
		return 0;
	s = '1'+s;
	SOCKADDR_IN client;
	client.sin_addr.S_un.S_addr = inet_addr(remoteServerIP.c_str());
	client.sin_family = AF_INET;
	client.sin_port = htons(remoteServerPort);
	sendto(sendSocket, (char*)s.GetBuffer(0), 2*s.GetLength() + 2, 0, (SOCKADDR*)&client, sizeof(SOCKADDR));

	return 1;
}

int SendMsgToClients(CString s)
{
	if (localMachineIsServerflag == 0)
		return 0;


	s = '1'+s;
	for (size_t i = 0; i < clientIpList.size(); i++)
	{
		SOCKADDR_IN client;
		client.sin_addr.S_un.S_addr = inet_addr(clientIpList[i].c_str());
		client.sin_family = AF_INET;
		client.sin_port = htons(clientPortList[i]);
		sendto(sendSocket, (char*)s.GetBuffer(0), 2 * s.GetLength() + 2, 0, (SOCKADDR*)&client, sizeof(SOCKADDR));//要2* 这个数字，因为是unicode 转 ascii
	}
	return 1;
}

CString RecvMsgFromServer()
{
	if (remoteServerIPflag == 0)
		return CString();

	char mesg[1024] = {};

	sockaddr_in addr;
	int len = sizeof(addr);
	int n = recvfrom(recvSocket, mesg, 1024, 0, (sockaddr *)&addr, &len);
	CString s((wchar_t*)mesg);
	if (n > 0)
	{
//		mesg[n + 1] = '\0';
		//过滤一开始的控制字符(unicode)
		return s.Right(s.GetLength()-1);
	}

	return CString();
}

//
//void test()
//{
//
//	recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if (recvSocket == INVALID_SOCKET)
//	{
//		printf("socket error !");
//		return ;
//	}
//
//	sockaddr_in serAddr;
//	serAddr.sin_family = AF_INET;
//	serAddr.sin_port = htons(5999);
//	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
//	if (bind(recvSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
//	{
//		printf("bind error !");
//		closesocket(recvSocket);
//		return ;
//	}
//	//	setnonblocking(serSocket);
//	sockaddr_in remoteAddr;
//	int nAddrLen = sizeof(remoteAddr);
//		char recvData[255];
//		int ret = recvfrom(recvSocket, recvData, 255, 0, (sockaddr *)&remoteAddr, &nAddrLen);
//		if (ret > 0)
//		{
//			recvData[ret] = 0x00;
//			printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
//			printf(recvData);
//		}
//
//		char * sendData = "一个来自服务端的UDP数据包\n";
//		sendto(recvSocket, sendData, strlen(sendData), 0, (sockaddr *)&remoteAddr, nAddrLen);
//
//
//}

CString RecvMsgFromClients()
{
	if (localMachineIsServerflag == 0)
		return CString();

	char mesg[1024] = {};
	sockaddr_in addr;
	int len = sizeof(addr);
	int n = recvfrom(recvSocket, mesg, 1024, 0, (sockaddr *)&addr, &len);
	CString s((wchar_t *) mesg);//+1过滤控制字符
	if (n>0 )//0代表对面发了自己的端口过来给你
	{
		if (s[0] == '0')
		{
			clientIpList.push_back(inet_ntoa(addr.sin_addr));
			string port = WChar2Ansi(s.GetBuffer(0));
			port = port.substr(port.find(":") + 1, port.npos);
			clientPortList.push_back(stringToInt(port));
			return CString();
		}
		else if(s[0]=='1')
		{

			return s.Right(s.GetLength() - 1);
		}

	}
//	test();



	return CString();
}

int setLocalMachineToServer(int port)
{


	//设置接收socket
	remoteServerIPflag = 0;
	localMachineIsServerflag = 1;
	localServerPort = port;

	recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(localServerPort);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	int ret = bind(recvSocket, (sockaddr*)&addr, sizeof(addr));
	if ( ret == -1)
	{
		localMachineIsServerflag = 0;
		return 0;
	}
	char recvData[255];
	int nAddrLen = sizeof(remoteAddr);//addrlend 一定要赋值才可以
//	int ret2 = recvfrom(recvSocket, recvData, 255, 0, (sockaddr *)&remoteAddr, &nAddrLen);

	setnonblocking(recvSocket);

	//设置发送socket
	sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	/*
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	*/
	clientIpList.clear();
	clientPortList.clear();

	return 1;
}
string intToString(int i)
{
	stringstream ss;
	ss << i;
	string r;
	ss >> r;
	return r;
}
int setServerIP(CString IP, int port)
{
	remoteServerIPflag = 1;
	localMachineIsServerflag = 0;
	remoteServerIP = WChar2Ansi(IP.GetBuffer(0));
	remoteServerPort = port;

	//设置监听端口
	recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN local;
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(0);

	bind(recvSocket, (SOCKADDR*)&local, sizeof(SOCKADDR));
	setnonblocking(recvSocket);
	struct sockaddr_in sin;
	int len2 = sizeof(sin);
	getsockname(recvSocket, (struct sockaddr *)&sin, &len2);
	int listenPort = ntohs(sin.sin_port);//获取绑定的随机端口号并将这个端口号发给 服务器，让服务器每次都往这个端口号发消息

	//设置服务器
	sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	remoteAddr.sin_addr.S_un.S_addr = inet_addr(WChar2Ansi(IP.GetBuffer(0)).c_str());//可能不对

	string msg = "0"+intToString(listenPort);//0代表要连接服务器；1，代表发消息；
	CString msg2(msg.c_str());
	int err = sendto(sendSocket, (char*)msg2.GetBuffer(0), 2*msg2.GetLength() + 2 , 0, (SOCKADDR*)&remoteAddr, sizeof(SOCKADDR));//为什么要乘以2，因为是unicode 转ascii

	return 1;
}



WSADATA wdata;
WORD wVersionRequired;
int initChatRoomSocket()
{
	wVersionRequired = MAKEWORD(2, 0);
	WSAStartup(wVersionRequired, &wdata);

	sendSocket = 0;
	recvSocket = 0;

	remoteServerIPflag = 0;
	localMachineIsServerflag = 0;

	clientIpList.clear();
	clientPortList.clear();

	return 0;
}
