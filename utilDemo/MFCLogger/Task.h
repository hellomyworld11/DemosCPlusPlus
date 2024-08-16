#pragma once


class Task
{
public:
	enum class Status
	{
		N,  //待执行
		E,	//异常
		Y   //完成
	};

public:
	//任务不能自己构造，必须从数据库读取
	static bool CreateTask(Task& task);

public:
	Task(){};
	Task(CString strid, CString strst);

	void Reset();

	~Task(void);

public:
	CString m_strGuid_;
	CString m_strflag_cad_;
	Status   m_st_;
};

