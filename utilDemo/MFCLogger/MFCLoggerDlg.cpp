
// MFCLoggerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCLogger.h"
#include "MFCLoggerDlg.h"
#include "afxdialogex.h"

#include <iostream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Logger.h"
#include "Task.h"
#include "function.h"

// CMFCLoggerDlg 对话框

CRichEditCtrl CMFCLoggerDlg::gOutput;

CMFCLoggerDlg::CMFCLoggerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCLoggerDlg::IDD, pParent),m_bRun(true),dTimeoutSec(30)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCLoggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, gOutput);
}

BEGIN_MESSAGE_MAP(CMFCLoggerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Btn_startCreo, &CMFCLoggerDlg::OnBnClickedBtnstartcreo)
	ON_BN_CLICKED(IDC_Btn_stopCreo, &CMFCLoggerDlg::OnBnClickedBtnstopcreo)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CMFCLoggerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_Btn_minotor, &CMFCLoggerDlg::OnBnClickedBtnminotor)
END_MESSAGE_MAP()


// CMFCLoggerDlg 消息处理程序

BOOL CMFCLoggerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CString strCompileTime;
	strCompileTime.Format(L"监控窗口 版本[%s %s]", CString(__DATE__), CString(__TIME__));
	SetWindowText(strCompileTime);

	// TODO: 在此添加额外的初始化代码
	LOG_DEBUG("监控系统启动...");

	LOG_DEBUG("初始化配置文件");


	CString strcreopath;

	m_client.SetPath(convert(strcreopath));

	LOG_DEBUG("启动目录: %s", convert(strcreopath).data());

	
	CString strTimeoutsec;
	
	dTimeoutSec = CStrToFloat(strTimeoutsec);
	
	m_thread = AfxBeginThread(BackMinotor, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCLoggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCLoggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCLoggerDlg::OnBnClickedBtnstartcreo()
{
	// TODO: 在此添加控件通知处理程序代码
	
	LOG_DEBUG("%d", -1);
	
}


void CMFCLoggerDlg::OnBnClickedBtnstopcreo()
{
	
}


void CMFCLoggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMERID_GETTASK:
		GetTask();
		break;
	case TIMERID_DOINGTIME:
		ComputeTime();
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


UINT CMFCLoggerDlg::BackMinotor(LPVOID param)
{
	CMFCLoggerDlg *pDlg = (CMFCLoggerDlg*)(param);
	if (pDlg == nullptr)
	{
		exit(-1);
	}
	//开始监控
	while (pDlg->m_bRun)
	{
		pDlg->CurTask.Reset();
		//判断进程是否存在
		if(!pDlg->m_client.IsStart())
		{	
			//获取任务，再判断要不要开启creo
			pDlg->SetTimer(TIMERID_GETTASK, 1000, NULL);
			pDlg->m_thread->SuspendThread();
		}else{

			CString strdoingguid;
			if (strdoingguid != L"")
			{
				pDlg->bComputfirst = true;
				pDlg->SetTimer(TIMERID_DOINGTIME, 5000, NULL);
				pDlg->m_thread->SuspendThread();
			}else{
				
			}
		}
	}
	return 0;
}

bool CMFCLoggerDlg::GetTask()
{
	KillTimer(TIMERID_GETTASK);
	
	if(!Task::CreateTask(CurTask))
	{
		//获取不到任务则隔一段时间一直获取
		SetTimer(TIMERID_GETTASK, 3000, NULL);
	}else{
		
		LOG_DEBUG("获取任务成功->任务id: [%s]", convert(CurTask.m_strGuid_).data());
		
		//设置 自动开启参数华为true  并且设置待执行任务id  给插件 使用
		

		//启动creo
		m_client.Start();

		// 任务状态改为正在执行，creo并进行操作，此时需要监控是否卡死 5s后进入卡死监控
		bComputfirst = true;
		SetTimer(TIMERID_DOINGTIME, 5000, NULL);

	}
	return true;
}

bool CMFCLoggerDlg::ComputeTime()
{
	//更改时间间隔
	KillTimer(TIMERID_DOINGTIME);
	CString strDoingGuid;
	if (bComputfirst)
	{
		bComputfirst = false; //第一次不读取执行状态， 下次开始读取正在执行状态

		if (strDoingGuid == L"")
		{	
			bComputfirst = true;
			SetTimer(TIMERID_DOINGTIME, 1000 * 5, NULL);
			LOG_DEBUG("没有正在执行的任务，隔5s再次监控");
			return true;
		}else{
			//已存在的情况，不用监控程序获取任务 直接读取ini文件获取正在执行任务id
			CurTask.m_strGuid_ = strDoingGuid;
		}
	}else{
	
		if (strDoingGuid == CurTask.m_strGuid_) //超时卡死，关闭creo进程 关闭定时器
		{
			m_client.Kill();

			
			//恢复阻塞线程
			m_thread->ResumeThread();
			return true;
		}else{
			;//任务正常完成，重新获取任务？
			//恢复阻塞线程
			m_thread->ResumeThread();
			return true;
		}
		
	}
	SetTimer(TIMERID_DOINGTIME, 1000 * dTimeoutSec, NULL);
	return true;
}


void CMFCLoggerDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bRun = false; //先退出后台监控子线程
	Sleep(100);
	CDialogEx::OnCancel();
}


void CMFCLoggerDlg::OnBnClickedBtnminotor()
{
	// TODO: 在此添加控件通知处理程序代码
}
