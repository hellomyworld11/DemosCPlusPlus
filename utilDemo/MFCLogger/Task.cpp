#include "stdafx.h"
#include "Task.h"

#include "function.h"

bool Task::CreateTask(Task& task)
{
	LOG_DEBUG("从数据库中获取一个待执行任务");

	vector<vector<_variant_t>> datas;
	
	if (datas.size() == 0)
	{
		return false;
	}
	_variant_t varTemp = datas[0][0];
	CString strTemp = (LPCTSTR)(_bstr_t)varTemp;
	CString strGuid = strTemp;					//任务标识：唯一标识

	varTemp = datas[0][1];
	strTemp = (LPCTSTR)(_bstr_t)varTemp;
	CString strFLAG_CAD = strTemp;					
	
	task = Task(strGuid, strFLAG_CAD);

	return true;
}



Task::Task(CString strid, CString strst):m_strflag_cad_(strst), m_strGuid_(strid)
{
	if (strid.MakeUpper() == L"N")
	{
		m_st_ = Status::N;
	}else if (strid.MakeUpper() == L"Y")
	{
		m_st_ = Status::Y;
	}else{
		m_st_ = Status::E;
	}
}

void Task::Reset()
{
	m_strflag_cad_ = L"";
	m_strGuid_ = L"";
	m_st_ = Status::N;
}

Task::~Task(void)
{
}
