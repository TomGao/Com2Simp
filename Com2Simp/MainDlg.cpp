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

	// ��ע
	CString remark = "[ע] �����������ת�����ַ�����Ҳ���������Ҫת�����ļ��С�\t\t\t\t\t\t\t\t\t\t��־�ļ���tools.log";
	SetDlgItemText(IDC_REMARK, remark);

	SetProcessInfo("׼������");

	//////////////////////////////////////////////////////////////////////////
	// ��ʼ������
	if(!m_cSB.Init(this))
	{
		SetProcessInfo(m_cSB.GetErrorMsg());
		return FALSE;
	}

	// ����������Ϣ
	if(!m_cSB.ReadConfig())
	{
		SetProcessInfo(m_cSB.GetErrorMsg());
		MessageBox("��ȡ�����ļ�ʧ�ܣ�\n" + m_cSB.GetErrorMsg(), "ת��", MB_OK|MB_ICONERROR);
	}

	// ����ϵͳ��Ϣ
	CString strInfo;
	strInfo.Format("%s V%s", m_cSB.GetTitle(), GetVerInfo());
	this->SetWindowText(strInfo);
	m_cSB.WriteLog(strInfo);

	SetDlgItemText(IDC_EDIT_SRC, m_cSB.GetSrcDir());
	SetDlgItemText(IDC_EDIT_DES, m_cSB.GetDesDir());

	m_combType = GetDlgItem(IDC_COMBO_CV_TYPE);
	m_combType.AddString("��ת��");
	m_combType.AddString("��ת��");
	m_combType.SetCurSel(m_cSB.GetCVType());

	((CButton)GetDlgItem(IDC_CHECK_DIR)).SetCheck(m_cSB.GetIsConvDir());

	return TRUE;
}

void CMainDlg::SetPercentInfo(int pos)
{
	CString msg;
	msg.Format("���ȣ�%d%%", pos);

	m_prog.SetPos(pos);
	SetDlgItemText(IDC_PERCENT_INFO, msg);
}

void CMainDlg::SetProcessInfo(CString msg)
{
	SetDlgItemText(IDC_INFO, "״̬��" + msg);
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
	SetProcessInfo("׼������");

	SPLITDATA sd;

	GetDlgItemText(IDC_EDIT_SRC, sd.sSrcDir);
	GetDlgItemText(IDC_EDIT_DES, sd.sDesDir);
	sd.eConvType = (CONV_TYPE)m_combType.GetCurSel();
	sd.nConvDir = ((CButton)GetDlgItem(IDC_CHECK_DIR)).GetCheck();

	// ��������
	m_cSB.SaveConfig(sd);

	bool bSuccess = false;
	CString sMsg;

	// 1.ת���ַ���
	CString sSrc;
	GetDlgItemText(IDC_EDIT_CUST, sSrc);
	sSrc.Trim();
	if (!sSrc.IsEmpty())
	{
		CString sDes;
		m_cSB.ConvertText(sSrc, sDes);
		SetDlgItemText(IDC_EDIT_CUST, sDes);
		sMsg = "ת���ı����";
		SetProcessInfo(sMsg);
		m_cSB.WriteLog(sMsg);
		bSuccess = true;
	}

	// 2.ת���ļ�(��)
	if(sd.nConvDir==1)
	{
		if (!m_cSB.SplitBill())
		{
			SetProcessInfo(m_cSB.GetErrorMsg());
			MessageBox("ת��ʧ�ܣ�\n" + m_cSB.GetErrorMsg(), "ת��", MB_OK|MB_ICONERROR);
			return 0;
		}
		bSuccess = true;
	}

	if (bSuccess)
	{
		SetPercentInfo(100);
		SetProcessInfo("ת�����");
		MessageBox(m_cSB.GetErrorMsg(), "ת��", MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		sMsg = "δ�����κ���Ҫת������";
		SetProcessInfo(sMsg);
		MessageBox(sMsg, "ת��", MB_OK|MB_ICONINFORMATION);
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
	if(BrowseDir("ԴĿ¼", dir))
	{
		SetDlgItemText(IDC_EDIT_SRC, dir);
	}
	else if(!dir.IsEmpty())
	{
		MessageBox("ת��ʧ�ܣ�\n" + dir, "ת��", MB_OK|MB_ICONERROR);
	}

	return 0;
}

LRESULT CMainDlg::OnBnClickedBrowseDES(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString dir;
	GetDlgItemText(IDC_EDIT_DES, dir);
	if(BrowseDir("Ŀ��Ŀ¼", dir))
	{
		SetDlgItemText(IDC_EDIT_DES, dir);
	}
	else if(!dir.IsEmpty())
	{
		MessageBox("ת��ʧ�ܣ�\n" + dir, "ת��", MB_OK|MB_ICONERROR);
	}

	return 0;
}