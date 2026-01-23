#include "CDlgAutoResize.h"

CDlgAutoResize::CDlgAutoResize()
	:m_pDlg(nullptr)
	,m_dXScale(1)
	,m_dYScale(1)
{

}

bool CDlgAutoResize::ManageCDlg(CDialog* pDlg)
{
	m_pDlg = pDlg;
	return Record();
}

bool CDlgAutoResize::Record()
{
	if (nullptr ==  m_pDlg)
	{
		return false;
	}

	m_oldRectMap.clear();

	m_pDlg->GetWindowRect(m_oldDlgScrRect);
	m_oldDlgCliRect = m_oldDlgScrRect;
	m_pDlg->ScreenToClient(m_oldDlgCliRect);

	CWnd* pChild = m_pDlg->GetWindow(GW_CHILD);
	CString strtext;
	while (pChild)
	{
		pChild->GetWindowText(strtext);	
		CRect rect;
		pChild->GetWindowRect(&rect);   // 获取控件屏幕坐标
		m_pDlg->ScreenToClient(&rect);  // 转换为对话框客户区坐标
		int nID = pChild->GetDlgCtrlID();
		m_oldRectMap[nID] =  rect ;
		pChild = pChild->GetNextWindow();	
	}
	return true;
}

bool CDlgAutoResize::RecordChildDlg(CDialog* pDlg)
{
	if (pDlg == nullptr)
	{
		return false;
	}
	

	CWnd *pParentWnd = pDlg->GetParent();
	if (pParentWnd == NULL)
	{
		return false;
	}
	wchar_t  className[256];
	::GetClassName(pParentWnd->m_hWnd, className, 256); // 获取类名
	CString strClass(className);


	CRect rectInParent;
	pParentWnd->GetClientRect(&rectInParent);
	rectInParent.left = rectInParent.top = 0;
	if (strClass.Find(L"tab")!=-1 || strClass.Find(L"Tab") != -1)
	{	
		rectInParent.top = GetTabTitleHeigth(pParentWnd);
	}

	int nID = pDlg->GetDlgCtrlID();

	m_oldChildDlg[nID] = pDlg;

	CRect oldDlgRect;
	CRect tmpRect;
	pDlg->MoveWindow(&rectInParent);
	pDlg->GetWindowRect(tmpRect);
	pParentWnd->ScreenToClient(tmpRect);
	oldDlgRect = tmpRect;
	m_OldChildDlgRectMap[nID] = oldDlgRect;
	return true;
}

bool CDlgAutoResize::Update(UINT nType, int cx, int cy)
{
	if (nullptr == m_pDlg)
	{
		return false;
	}
	
	if (nType == SIZE_MINIMIZED || m_oldRectMap.empty())
		return false;

	//计算缩放比例
	CRect curRectCli;
	m_pDlg->GetClientRect(curRectCli);

	if (m_oldDlgCliRect.Width() == 0 || m_oldDlgCliRect.Height() == 0)
		return false;

	float fScaleX = (float)curRectCli.Width() / (float)m_oldDlgCliRect.Width();
	float fScaleY = (float)curRectCli.Height() / (float)m_oldDlgCliRect.Height();
	m_dXScale = fScaleX; // 保存比例（如果需要）
	m_dYScale = fScaleY;

	for(auto& apair : m_oldRectMap)
	{
		int nCtrlID = apair.first;
		CRect rectOldCli = apair.second;
		CWnd* pCtrl = nullptr;
		
		pCtrl = m_pDlg->GetDlgItem(nCtrlID);

		if (pCtrl && ::IsWindow(pCtrl->GetSafeHwnd()))
		{
			// 根据比例计算新的位置和大小		
			CRect rectNewCli;
			GetWndNewRect(pCtrl, fScaleX, fScaleY, rectOldCli, rectNewCli);

			// 3. 应用新矩形到控件
			pCtrl->MoveWindow(rectNewCli);

		//	CWnd *pSubWnd = pCtrl->GetWindow(GW_CHILD);
		//	if (pSubWnd)
		//	{
		//		UpdateRecur(pCtrl, nType, cx, cy);
		//	}	
		}
	}

	for (auto& apair : m_OldChildDlgRectMap)
	{
		int nCtrlID = apair.first;
		CRect rectOldCli = apair.second;
		CWnd* pCtrl = m_oldChildDlg[nCtrlID];
		if (pCtrl && ::IsWindow(pCtrl->GetSafeHwnd()))
		{
			// 根据比例计算新的位置和大小		
			CRect rectNewCli;
			GetWndNewRect(pCtrl, fScaleX, fScaleY, rectOldCli, rectNewCli);
			rectNewCli.left = rectOldCli.left;
			rectNewCli.top = rectOldCli.top;
			// 3. 应用新矩形到控件
			pCtrl->MoveWindow(rectNewCli);
		}
	}

	m_pDlg->RedrawWindow();
	return true;
}

bool CDlgAutoResize::UpdateRecur(CWnd* pParentWnd, UINT nType, int cx, int cy)
{
	CWnd* pChild = pParentWnd->GetWindow(GW_CHILD);
	while (pChild)
	{
		CRect rect;
		pChild->GetWindowRect(&rect);   // 获取控件屏幕坐标
		m_pDlg->ScreenToClient(&rect);  // 转换为对话框客户区坐标
		UINT nID = pChild->GetDlgCtrlID();
		//m_oldRectMap[nID] = rect;
		auto iter = m_oldRectMap.find(nID);
		if (iter == m_oldRectMap.end())
		{
			pChild = pChild->GetNextWindow();
			continue;
		}
		CRect rectNewCli;
		GetWndNewRect(pChild, m_dXScale, m_dYScale, iter->second, rectNewCli);

		// 3. 应用新矩形到控件
		pChild->MoveWindow(rectNewCli);

		::SendMessage(pChild->m_hWnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.Width(), rect.Height()));
		//pChild->SendMessage(WM_SIZE, SIZE_RESTORED,
		//	MAKELPARAM(rect.Width(), rect.Height()));
		CWnd *pSubWnd = pChild->GetWindow(GW_CHILD);
		if (pSubWnd)
		{
			UpdateRecur(pChild, nType, cx, cy);
		}
		pChild = pChild->GetNextWindow();
	}
	return true;
}

bool CDlgAutoResize::RecordRecur(CWnd* pParentWnd, int parentId)
{
	CWnd* pChild = pParentWnd->GetWindow(GW_CHILD);
	while (pChild)
	{
		CRect rect;
		pChild->GetWindowRect(&rect);   // 获取控件屏幕坐标
		m_pDlg->ScreenToClient(&rect);  // 转换为对话框客户区坐标
		int nID = pChild->GetDlgCtrlID();
		m_oldRectMap[nID] = rect;
		CWnd *pSubWnd = pChild->GetWindow(GW_CHILD);
		if (pSubWnd)
		{
			RecordRecur(pChild, nID);
		}
		pChild = pChild->GetNextWindow();
	}
	return true;
}

bool CDlgAutoResize::GetWndNewRect(CWnd*&pWnd, float dxScale, float dyScale, CRect oldCliRect, CRect& newCliRect)
{
	newCliRect.left = (LONG)(oldCliRect.left   * dxScale);
	newCliRect.top = (LONG)(oldCliRect.top    * dyScale);
	newCliRect.right = (LONG)(oldCliRect.right  * dxScale);
	newCliRect.bottom = (LONG)(oldCliRect.bottom * dyScale);
	return true;
}

float CDlgAutoResize::GetTabTitleHeigth(CWnd*& pWnd)
{
	RECT tabtitlerect;
	TabCtrl_GetItemRect(pWnd->m_hWnd, 0, &tabtitlerect);
	return tabtitlerect.bottom - tabtitlerect.top;
}
