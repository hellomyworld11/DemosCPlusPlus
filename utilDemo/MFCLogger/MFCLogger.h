
//PROJECT_NAME Ӧ�ó������ͷ�ļ�


#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCLoggerApp:
// �йش����ʵ�֣������ MFCLogger.cpp
//

class CMFCLoggerApp : public CWinApp
{
public:
	CMFCLoggerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCLoggerApp theApp;