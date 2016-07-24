#ifndef _KSSPLITBILL
#define _KSSPLITBILL

#include "SplitData.h"
#include "Log.h"

class CMainDlg;

class CSplitBill
{
public:
	CSplitBill();
public:
	~CSplitBill(void);

public:

	bool Init(CMainDlg* pMainDlg);

	void WriteLog(CString log);

	bool ReadConfig();

	bool SaveConfig(SPLITDATA& sd);

	// ��ʼת��
	bool SplitBill();

	CString GetSrcDir();

	CString GetDesDir();

	CString GetTitle();

	int GetIsConvDir();

	int GetCVType();

	CString GetErrorMsg();

	// ת���ַ���
	bool ConvertText(const CString& sSrc, CString& sDes);

private:
	// �ݹ�����Դ�ļ�����
	int LoadData(S_DIR* sDir, CString sSrcDir, CString sDesDir);

	// �ݹ�����Ŀ���ļ�����
	bool BuildData(const S_DIR* sDir);

	// ת���ļ�
	bool Convert(CString srcFile, CString desFile);

private:

	int m_nErrorCode;
	CString	m_sErrorMsg;
	CLog	m_log;

	// �洢������
	CSplitData m_cSD;

	CMainDlg* m_pMainDlg;

	CString m_sLogFile;

	CString m_sTitle;

	// �ܹ���Ҫת��������
	int m_nConvCount;

	// ��ǰת���ĺ���
	int m_nConvNum;
};

#endif  