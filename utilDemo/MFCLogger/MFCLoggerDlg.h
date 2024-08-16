#pragma once
#include "resource.h"
#include "ExeClient.h"
#include "Task.h"
#include "afxwin.h"

//#define TIMERID_FINDCREO    1
#define TIMERID_GETTASK		1
#define TIMERID_DOINGTIME   2


//监控程序对话框
class CMFCLoggerDlg : public CDialogEx
{
// 构造
public:
	CMFCLoggerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCLogger_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstartcreo();

	afx_msg void OnBnClickedBtnstopcreo();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	
	
	static UINT BackMinotor(LPVOID param);

	//定时器子函数
	bool GetTask();

	bool ComputeTime();

public:
	ExeClient m_client;
	
	CWinThread*	m_thread;

	bool m_bRun;

	float dTimeoutSec;

	bool bComputfirst;

	Task CurTask;
	afx_msg void OnBnClickedCancel();

	static CRichEditCtrl gOutput;
	afx_msg void OnBnClickedBtnminotor();
};
