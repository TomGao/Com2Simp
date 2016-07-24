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

	// 开始转换
	bool SplitBill();

	CString GetSrcDir();

	CString GetDesDir();

	CString GetTitle();

	int GetIsConvDir();

	int GetCVType();

	CString GetErrorMsg();

	// 转换字符串
	bool ConvertText(const CString& sSrc, CString& sDes);

private:
	// 递归生成源文件集合
	int LoadData(S_DIR* sDir, CString sSrcDir, CString sDesDir);

	// 递归生成目标文件集合
	bool BuildData(const S_DIR* sDir);

	// 转换文件
	bool Convert(CString srcFile, CString desFile);

private:

	int m_nErrorCode;
	CString	m_sErrorMsg;
	CLog	m_log;

	// 存储的数据
	CSplitData m_cSD;

	CMainDlg* m_pMainDlg;

	CString m_sLogFile;

	CString m_sTitle;

	// 总共需要转换的数量
	int m_nConvCount;

	// 当前转换的号码
	int m_nConvNum;
};

#endif  