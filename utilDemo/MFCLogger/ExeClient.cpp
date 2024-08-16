#include "stdafx.h"
#include "ExeClient.h"
#include "tlhelp32.h"




ExeClient::ExeClient(std::string strpath):strBatPath_(strpath)
{

}

ExeClient::ExeClient()
{
	
}

ExeClient::~ExeClient(void)
{
}

void ExeClient::SetPath(std::string strpath)
{
	strBatPath_ = strpath;
}

bool ExeClient::Start()
{
	//start

	if (false)
	{
		LOG_DEBUG("start creo error : %d", -1);
		return false;
	}

	return true;
}

bool ExeClient::Stop()
{
	
	return true;
}

tuple<bool, std::string, UINT> ExeClient::GetProcess()
{
	std::tuple<bool, std::string, UINT> ret(false, "", 0);
	CString strCreo = L"xtop.exe";
	//获取进程快照
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);//进程快照
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return ret;
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(PROCESSENTRY32);
	BOOL bResult=Process32First(hProcessSnap,&pe32);
	CString strExEName,strProcessId;
	UINT processid = 0;
	int CreoNum = 0;
	while (bResult)
	{
		strExEName = pe32.szExeFile;
		strProcessId.Format(L"%d",pe32.th32ProcessID);
		processid = pe32.th32ProcessID;
		if (strCreo == strExEName)//判断该进程是否目标进程
		{
			CreoNum++;
			break;
		}
		bResult=Process32Next(hProcessSnap,&pe32);
	}

	if(CreoNum > 0)
	{
		std::get<0>(ret) = true;
		std::get<1>(ret) = CT2A((LPCTSTR)strExEName);;
		std::get<2>(ret) = processid;
	}

	return ret;
}

bool ExeClient::Kill()
{
	tuple<bool, std::string, UINT> ret = GetProcess();
	if (!std::get<0>(ret))
	{
		return false;
	}

	return KillProcess(std::get<2>(ret));
}

bool ExeClient::Connect()
{
	unsigned int sec = 5;
	return false;
}

bool ExeClient::IsStart()
{
	tuple<bool, std::string, UINT> ret = GetProcess();
	return std::get<0>(ret);
}

bool ExeClient::KillProcess(DWORD pid)
{
	//打开进程得到进程句柄
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE, pid);
	if(hProcess==NULL)
	{ 		
		printf("OpenProcess error\n");
		return false;
	}
	//结束进程
	if (TerminateProcess(hProcess,0))
	{
		printf("结束进程成功\n");
		return true;
	}
	else
	{
		printf("结束进程失败\n");
		return false;
	}	
	
}
