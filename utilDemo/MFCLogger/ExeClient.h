#pragma once
#include "stdafx.h"
#include <tuple>

//��ص�Ӧ�ó���
class ExeClient
{
public:
	ExeClient();

	ExeClient(std::string strpath);

	~ExeClient(void);

	void SetPath(std::string strpath);

	bool Start();

	bool Stop();

	bool Kill();
	
	bool Connect();
	
	bool IsStart();
private:
	tuple<bool, std::string, UINT> GetProcess();

	bool KillProcess(DWORD pid);
private:
	std::string strBatPath_;
};

