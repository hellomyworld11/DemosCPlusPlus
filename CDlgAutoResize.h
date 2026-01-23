#pragma once
#include "stdafx.h"

class CDlgAutoResize 
{
public:
	explicit CDlgAutoResize();

	bool ManageCDlg(CDialog* pDlg);

	bool Record();	

	bool RecordChildDlg(CDialog* pDlg);

	bool Update(UINT nType, int cx, int cy);

	bool UpdateRecur(CWnd* pParentWnd, UINT nType, int cx, int cy);

	bool RecordRecur(CWnd* pParentWnd, int parentId = -1);

	float GetScaleX() { return m_dXScale; }

	float GetScaleY() { return m_dYScale; }
private:
	bool GetWndNewRect(CWnd*&pWnd, float dxScale, float dyScale, CRect oldCliRect, CRect& newCliRect);
	
	float GetTabTitleHeigth(CWnd*& pWnd);
private:
	CDialog* m_pDlg;

	CRect m_oldDlgScrRect; 

	CRect m_oldDlgCliRect;

	map<int, CRect> m_oldRectMap;  //客户区坐标

	map<int, CRect> m_OldChildDlgRectMap;
	map<int, CDialog*> m_oldChildDlg;
//	map<int, pair<int, CRect>> m_oldRectMap;//客户区坐标  子控件id 子控件的父控件-rect

	float m_dXScale;

	float m_dYScale;

//	vector<CDialog*> m_arrChildDlg;
};


