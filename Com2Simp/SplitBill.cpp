#include "StdAfx.h"
#include "SplitBill.h"
#include "common.h"

#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#include "MainDlg.h"

CSplitBill::CSplitBill(void)
{
	m_nErrorCode = 0;
	m_sErrorMsg = "未知错误！";
}

CSplitBill::~CSplitBill(void)
{
}

bool CSplitBill::Init(CMainDlg* pMainDlg)
{
	m_pMainDlg = pMainDlg;

	char sDir[1024] = {0};
	GetCurrentDirectory(1024, sDir);

	m_sLogFile.Format("%s\\tools.log", sDir);
	if(_access(m_sLogFile,0)==0 && FALSE == DeleteFile(m_sLogFile))
	{
		m_nErrorCode = GetLastError();
		m_sErrorMsg = "转换系统删除日志文件出错：" + GetErrorMsg() + "\n" + m_sLogFile;
		WriteLog(m_sErrorMsg);
		return false;
	}

	if (!m_log.Open(m_sLogFile))
	{
		m_nErrorCode =0;
		m_sErrorMsg = "打开日志文件出错\n" + m_sLogFile;
		WriteLog(m_sErrorMsg);
		return false;
	}

	return true;
}

void CSplitBill::WriteLog(CString log)
{
	char sTemp[100]={0};
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	sprintf_s(sTemp, "%d-%02d-%02d %02d:%02d:%02d", 
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond);

	CString sLog;
	sLog.Format("%s %s", sTemp, log);

	m_log.WriteLog(sLog);
}

CString CSplitBill::GetErrorMsg()
{
	// 非0，则为系统错误
	if (m_nErrorCode != 0)
	{
		LPVOID lpMsgBuf;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, m_nErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR)&lpMsgBuf, 0, NULL);

		m_sErrorMsg.Format("%d : %s", m_nErrorCode, lpMsgBuf);

		LocalFree(lpMsgBuf);
	}

	return m_sErrorMsg;
}

bool CSplitBill::ReadConfig()
{
	bool bRet = false;

	CString strPath = ".\\Config.ini";
	const int STR_SIZE = 102400;
	char strTemp[STR_SIZE] = {0};

	GetPrivateProfileString("COMMON", "TITLE", "[*****]工具", strTemp, STR_SIZE, strPath);
	m_sTitle = strTemp;

	GetPrivateProfileString("COMMON", "SRCDIR", "C:\\Test\\Split\\src\\", strTemp, STR_SIZE, strPath);
	m_cSD.m_sSD.sSrcDir = strTemp;

	GetPrivateProfileString("COMMON", "DESDIR", "C:\\Test\\Split\\des\\", strTemp, STR_SIZE, strPath);
	m_cSD.m_sSD.sDesDir = strTemp;

	m_cSD.m_sSD.nConvDir = GetPrivateProfileInt("COMMON", "ISCONVDIR", 1, strPath);

	m_cSD.m_sSD.eConvType = (CONV_TYPE)GetPrivateProfileInt("COMMON", "ISCOM2SIMP", 1, strPath);

	return true;
}	

bool CSplitBill::SaveConfig(SPLITDATA& sd)
{
	CString strPath = ".\\Config.ini";

	sd.sSrcDir.Trim();
	sd.sDesDir.Trim();

	// 去掉最后的‘\’
	if(!sd.sSrcDir.IsEmpty() && sd.sSrcDir.Right(1) == "\\")
		sd.sSrcDir = sd.sSrcDir.Left(sd.sSrcDir.GetLength()-1);

	if(!sd.sDesDir.IsEmpty() && sd.sDesDir.Right(1) == "\\")
		sd.sDesDir = sd.sDesDir.Left(sd.sDesDir.GetLength()-1);

	WritePrivateProfileString("COMMON", "SRCDIR", sd.sSrcDir, strPath);
	WritePrivateProfileString("COMMON", "DESDIR", sd.sDesDir, strPath);

	CString sTemp;
	sTemp.Format("%d", sd.eConvType);
	WritePrivateProfileString("COMMON", "ISCOM2SIMP", sTemp, strPath);

	sTemp.Format("%d", sd.nConvDir);
	WritePrivateProfileString("COMMON", "ISCONVDIR", sTemp, strPath);

	m_cSD.m_sSD = sd;
	m_nErrorCode = 0;
	m_sErrorMsg = "转换完成";

	return true;
}


CString CSplitBill::GetSrcDir()
{
	return m_cSD.m_sSD.sSrcDir;
}

CString CSplitBill::GetDesDir()
{
	return m_cSD.m_sSD.sDesDir;
}

int CSplitBill::GetIsConvDir()
{
	return m_cSD.m_sSD.nConvDir;
}

CString CSplitBill::GetTitle()
{
	return m_sTitle;
}

int CSplitBill::GetCVType()
{
	return m_cSD.m_sSD.eConvType;
}

bool CSplitBill::SplitBill()
{
	bool bRet = false;

	// 先检查下源目录和目标目录是否存在
	if(m_cSD.m_sSD.sSrcDir.IsEmpty() || _access(m_cSD.m_sSD.sSrcDir, 0 ) == -1)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("源目录[%s]不存在", m_cSD.m_sSD.sSrcDir);
		return bRet;
	}
	if(m_cSD.m_sSD.sDesDir.IsEmpty() || _access(m_cSD.m_sSD.sDesDir, 0 ) == -1)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("目标目录[%s]不存在", m_cSD.m_sSD.sDesDir);
		return bRet;
	}
	if(m_cSD.m_sSD.sSrcDir.CompareNoCase(m_cSD.m_sSD.sDesDir) == 0)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("源目录不能与目标目录相同！");
		return bRet;
	}

	DWORD dwStart = GetTickCount();

	// 加载数据
	WriteLog("加载数据");
	m_nConvCount = LoadData(&m_cSD.m_dir, m_cSD.m_sSD.sSrcDir, m_cSD.m_sSD.sDesDir);

	// 执行数据
	m_nConvNum = 0;
	WriteLog("----开始转换");
	bRet = BuildData(&m_cSD.m_dir);
	WriteLog("----结束转换");

	m_nErrorCode = 0;
	m_sErrorMsg.Format("转换完成!\n总共[%d]个文件，成功转换[%d]个文件，耗时[%d]ms", m_nConvCount, m_nConvNum, GetTickCount()-dwStart);
	WriteLog(m_sErrorMsg);

	return bRet;
}

// 递归生成源文件集合
int CSplitBill::LoadData(S_DIR* sDir, CString sSrcDir, CString sDesDir)
{
	int nCount = 0;

	// 目录赋值
	sDir->Clear();
	sDir->ssrcDirName = sSrcDir;
	sDir->sdesDirName = sDesDir;

	long lf;
	_finddata_t file;
	if((lf = _findfirst(sSrcDir + "\\*.*", &file))==-1l)
	{
		_findclose(lf);
		return nCount;
	}

	while( _findnext( lf, &file ) == 0 )
	{
		CString filename(file.name);

		// 当前目录或父目录
		if (filename.CompareNoCase(".")==0 || filename.CompareNoCase("..")==0)
			continue; 

		// 目录
		if(_A_SUBDIR == file.attrib)
		{
			S_DIR sDirChild;
			int cnt = LoadData(&sDirChild, sSrcDir+"\\"+filename, sDesDir+"\\"+filename);

			if (cnt > 0)
			{
				sDir->lsDir.push_back(sDirChild);
				nCount += cnt;
			}
		}
		else // 文件
		{
			sDir->lsFilename.push_back(filename);
			++nCount;
		}
	}

	_findclose(lf);

	return nCount;
}

// 递归生成目标文件集合
bool CSplitBill::BuildData(const S_DIR* sDir)
{
	bool bRet = false;

	// 检查目标目录，不存在，则创建
	if( (_access(sDir->sdesDirName, 0 )) != 0)
	{
		// 再增加
		if(FALSE == CreateDirectory(sDir->sdesDirName, NULL))
		{
			m_nErrorCode = GetLastError();
			return bRet;
		}
	}

	// 开始循环转换文件
	list<CString>::const_iterator iterFile = sDir->lsFilename.begin();
	while(iterFile != sDir->lsFilename.end())
	{
		CString name = *iterFile++;
		CString srcFile = sDir->ssrcDirName + "\\" + name;
		CString desFile = sDir->sdesDirName + "\\" + name;

		Convert(srcFile, desFile);
	}

	list<S_DIR>::const_iterator iterDir = sDir->lsDir.begin();
	while(iterDir != sDir->lsDir.end())
	{
		const S_DIR* pdir = &(*iterDir);
		if(!BuildData(pdir))
			return bRet;

		++iterDir;
	}

	return true;
}

bool CSplitBill::ConvertText(const CString& sSrc, CString& sDes)
{
	char buffer[102400] = {0};
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, sSrc);

	if (m_cSD.m_sSD.eConvType == CT_C2S)
	{
		GBK2GB(buffer);
	}
	else if (m_cSD.m_sSD.eConvType == CT_S2C)
	{
		GB2GBK(buffer);
	}

	sDes = CString(buffer);

	return true;
}

bool CSplitBill::Convert(CString srcFile, CString desFile)
{
	bool bRet = false;

	// 写日志
	CString sMsg;
	sMsg.Format("转换文件[%s]->[%s]", srcFile, desFile);
	WriteLog(sMsg);
	m_pMainDlg->SetProcessInfo(sMsg);
	m_pMainDlg->SetPercentInfo((++m_nConvNum * 100) / m_nConvCount);

	// 检查源文件
	if( (_access(srcFile, 0 )) == -1)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("源文件[%s]不存在", srcFile);
		WriteLog("发生错误：" + m_sErrorMsg);
		return bRet;
	}

	// 目标文件如果存在，则删除
	if( (_access(desFile, 0 )) == 0)
	{
		if(!DeleteFile(desFile))
		{
			m_nErrorCode = 0;
			m_sErrorMsg.Format("删除目标文件[%s]失败", desFile);
			WriteLog("发生错误：" + m_sErrorMsg);
			return bRet;
		}
	}

	// 打开源文件（只读）
	FILE* pFile;
	if (NULL == (pFile = ::fopen(srcFile, "r")))
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("txt文件打开失败:%s", srcFile);
		WriteLog("发生错误：" + m_sErrorMsg);
		return bRet;
	}

	// 打开目标文件
	FILE* pDesFile;
	if (NULL == (pDesFile = ::fopen(desFile, "a+")))
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("txt文件打开失败:%s", desFile);
		WriteLog("发生错误：" + m_sErrorMsg);

		::fclose(pFile);
		return bRet;
	}

	CString strLine;
	while (0 == ReadLine(pFile, strLine))
	{
		CString sDesLine;
		ConvertText(strLine, sDesLine);

		::fwrite(sDesLine, sizeof(char), sDesLine.GetLength(), pDesFile);
		::fwrite("\n", sizeof(char), 1, pDesFile);
	} 

	::fclose(pDesFile);
	::fclose(pFile);

	return true;
}