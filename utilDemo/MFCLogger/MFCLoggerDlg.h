#pragma once
#include "resource.h"
#include "ExeClient.h"
#include "Task.h"
#include "afxwin.h"

//#define TIMERID_FINDCREO    1
#define TIMERID_GETTASK		1
#define TIMERID_DOINGTIME   2


//��س���Ի���
class CMFCLoggerDlg : public CDialogEx
{
// ����
public:
	CMFCLoggerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCLogger_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstartcreo();

	afx_msg void OnBnClickedBtnstopcreo();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	
	
	static UINT BackMinotor(LPVOID param);

	//��ʱ���Ӻ���
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
