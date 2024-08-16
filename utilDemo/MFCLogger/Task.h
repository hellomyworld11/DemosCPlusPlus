#pragma once


class Task
{
public:
	enum class Status
	{
		N,  //��ִ��
		E,	//�쳣
		Y   //���
	};

public:
	//�������Լ����죬��������ݿ��ȡ
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

