// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "common.h"


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	//HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
	//	IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	//////////////////////////////////////////////////////////////////////////
	m_prog = GetDlgItem(IDC_PROGRESS1);
	m_prog.SetRange(0, 100);
	SetPercentInfo(0);

	// 备注
	CString remark = "[注] 您可以输入待转换的字符串，也可以浏览需要转换的文件夹。\t\t\t\t\t\t\t\t\t\t日志文件：tools.log";
	SetDlgItemText(IDC_REMARK, remark);

	SetProcessInfo("准备就绪");

	//////////////////////////////////////////////////////////////////////////
	// 初始化对象
	if(!m_cSB.Init(this))
	{
		SetProcessInfo(m_cSB.GetErrorMsg());
		return FALSE;
	}

	// 加载配置信息
	if(!m_cSB.ReadConfig())
	{
		SetProcessInfo(m_cSB.GetErrorMsg());
		MessageBox("读取配置文件失败！\n" + m_cSB.GetErrorMsg(), "转换", MB_OK|MB_ICONERROR);
	}

	// 加载系统信息
	CString strInfo;
	strInfo.Format("%s V%s", m_cSB.GetTitle(), GetVerInfo());
	this->SetWindowText(strInfo);
	m_cSB.WriteLog(strInfo);

	SetDlgItemText(IDC_EDIT_SRC, m_cSB.GetSrcDir());
	SetDlgItemText(IDC_EDIT_DES, m_cSB.GetDesDir());

	m_combType = GetDlgItem(IDC_COMBO_CV_TYPE);
	m_combType.AddString("繁转简");
	m_combType.AddString("简转繁");
	m_combType.SetCurSel(m_cSB.GetCVType());

	((CButton)GetDlgItem(IDC_CHECK_DIR)).SetCheck(m_cSB.GetIsConvDir());

	return TRUE;
}

void CMainDlg::SetPercentInfo(int pos)
{
	CString msg;
	msg.Format("进度：%d%%", pos);

	m_prog.SetPos(pos);
	SetDlgItemText(IDC_PERCENT_INFO, msg);
}

void CMainDlg::SetProcessInfo(CString msg)
{
	SetDlgItemText(IDC_INFO, "状态：" + msg);
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	SetPercentInfo(0);
	SetProcessInfo("准备就绪");

	SPLITDATA sd;

	GetDlgItemText(IDC_EDIT_SRC, sd.sSrcDir);
	GetDlgItemText(IDC_EDIT_DES, sd.sDesDir);
	sd.eConvType = (CONV_TYPE)m_combType.GetCurSel();
	sd.nConvDir = ((CButton)GetDlgItem(IDC_CHECK_DIR)).GetCheck();

	// 保存配置
	m_cSB.SaveConfig(sd);

	bool bSuccess = false;
	CString sMsg;

	// 1.转换字符串
	CString sSrc;
	GetDlgItemText(IDC_EDIT_CUST, sSrc);
	sSrc.Trim();
	if (!sSrc.IsEmpty())
	{
		CString sDes;
		m_cSB.ConvertText(sSrc, sDes);
		SetDlgItemText(IDC_EDIT_CUST, sDes);
		sMsg = "转换文本完成";
		SetProcessInfo(sMsg);
		m_cSB.WriteLog(sMsg);
		bSuccess = true;
	}

	// 2.转换文件(夹)
	if(sd.nConvDir==1)
	{
		if (!m_cSB.SplitBill())
		{
			SetProcessInfo(m_cSB.GetErrorMsg());
			MessageBox("转换失败！\n" + m_cSB.GetErrorMsg(), "转换", MB_OK|MB_ICONERROR);
			return 0;
		}
		bSuccess = true;
	}

	if (bSuccess)
	{
		SetPercentInfo(100);
		SetProcessInfo("转换完成");
		MessageBox(m_cSB.GetErrorMsg(), "转换", MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		sMsg = "未发现任何需要转换数据";
		SetProcessInfo(sMsg);
		MessageBox(sMsg, "转换", MB_OK|MB_ICONINFORMATION);
	}

	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnBnClickedBrowseSRC(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString dir;
	GetDlgItemText(IDC_EDIT_SRC, dir);
	if(BrowseDir("源目录", dir))
	{
		SetDlgItemText(IDC_EDIT_SRC, dir);
	}
	else if(!dir.IsEmpty())
	{
		MessageBox("转换失败！\n" + dir, "转换", MB_OK|MB_ICONERROR);
	}

	return 0;
}

LRESULT CMainDlg::OnBnClickedBrowseDES(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString dir;
	GetDlgItemText(IDC_EDIT_DES, dir);
	if(BrowseDir("目标目录", dir))
	{
		SetDlgItemText(IDC_EDIT_DES, dir);
	}
	else if(!dir.IsEmpty())
	{
		MessageBox("转换失败！\n" + dir, "转换", MB_OK|MB_ICONERROR);
	}

	return 0;
}