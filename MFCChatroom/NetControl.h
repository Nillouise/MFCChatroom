#pragma once
#include "stdafx.h"
#include<string>

int initChatRoomSocket();
int setServerIP(CString IP, int port);

int setLocalMachineToServer(int port);
CString RecvMsgFromClients();
CString RecvMsgFromServer();
int SendMsgToClients(CString s);
int SendMsgToServer(CString s);
std::string WChar2Ansi(LPCWSTR pwszSrc);