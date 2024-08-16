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
	//��ȡ���̿���
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);//���̿���
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
		if (strCreo == strExEName)//�жϸý����Ƿ�Ŀ�����
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
	//�򿪽��̵õ����̾��
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE, pid);
	if(hProcess==NULL)
	{ 		
		printf("OpenProcess error\n");
		return false;
	}
	//��������
	if (TerminateProcess(hProcess,0))
	{
		printf("�������̳ɹ�\n");
		return true;
	}
	else
	{
		printf("��������ʧ��\n");
		return false;
	}	
	
}
