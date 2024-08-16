
// MFCLoggerDlg.cpp : ʵ���ļ�
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

// CMFCLoggerDlg �Ի���

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


// CMFCLoggerDlg ��Ϣ�������

BOOL CMFCLoggerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	CString strCompileTime;
	strCompileTime.Format(L"��ش��� �汾[%s %s]", CString(__DATE__), CString(__TIME__));
	SetWindowText(strCompileTime);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	LOG_DEBUG("���ϵͳ����...");

	LOG_DEBUG("��ʼ�������ļ�");


	CString strcreopath;

	m_client.SetPath(convert(strcreopath));

	LOG_DEBUG("����Ŀ¼: %s", convert(strcreopath).data());

	
	CString strTimeoutsec;
	
	dTimeoutSec = CStrToFloat(strTimeoutsec);
	
	m_thread = AfxBeginThread(BackMinotor, this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCLoggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCLoggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCLoggerDlg::OnBnClickedBtnstartcreo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
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
	//��ʼ���
	while (pDlg->m_bRun)
	{
		pDlg->CurTask.Reset();
		//�жϽ����Ƿ����
		if(!pDlg->m_client.IsStart())
		{	
			//��ȡ�������ж�Ҫ��Ҫ����creo
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
		//��ȡ�����������һ��ʱ��һֱ��ȡ
		SetTimer(TIMERID_GETTASK, 3000, NULL);
	}else{
		
		LOG_DEBUG("��ȡ����ɹ�->����id: [%s]", convert(CurTask.m_strGuid_).data());
		
		//���� �Զ�����������Ϊtrue  �������ô�ִ������id  ����� ʹ��
		

		//����creo
		m_client.Start();

		// ����״̬��Ϊ����ִ�У�creo�����в�������ʱ��Ҫ����Ƿ��� 5s����뿨�����
		bComputfirst = true;
		SetTimer(TIMERID_DOINGTIME, 5000, NULL);

	}
	return true;
}

bool CMFCLoggerDlg::ComputeTime()
{
	//����ʱ����
	KillTimer(TIMERID_DOINGTIME);
	CString strDoingGuid;
	if (bComputfirst)
	{
		bComputfirst = false; //��һ�β���ȡִ��״̬�� �´ο�ʼ��ȡ����ִ��״̬

		if (strDoingGuid == L"")
		{	
			bComputfirst = true;
			SetTimer(TIMERID_DOINGTIME, 1000 * 5, NULL);
			LOG_DEBUG("û������ִ�е����񣬸�5s�ٴμ��");
			return true;
		}else{
			//�Ѵ��ڵ���������ü�س����ȡ���� ֱ�Ӷ�ȡini�ļ���ȡ����ִ������id
			CurTask.m_strGuid_ = strDoingGuid;
		}
	}else{
	
		if (strDoingGuid == CurTask.m_strGuid_) //��ʱ�������ر�creo���� �رն�ʱ��
		{
			m_client.Kill();

			
			//�ָ������߳�
			m_thread->ResumeThread();
			return true;
		}else{
			;//����������ɣ����»�ȡ����
			//�ָ������߳�
			m_thread->ResumeThread();
			return true;
		}
		
	}
	SetTimer(TIMERID_DOINGTIME, 1000 * dTimeoutSec, NULL);
	return true;
}


void CMFCLoggerDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bRun = false; //���˳���̨������߳�
	Sleep(100);
	CDialogEx::OnCancel();
}


void CMFCLoggerDlg::OnBnClickedBtnminotor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
