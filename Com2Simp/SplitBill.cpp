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
	m_sErrorMsg = "δ֪����";
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
		m_sErrorMsg = "ת��ϵͳɾ����־�ļ�����" + GetErrorMsg() + "\n" + m_sLogFile;
		WriteLog(m_sErrorMsg);
		return false;
	}

	if (!m_log.Open(m_sLogFile))
	{
		m_nErrorCode =0;
		m_sErrorMsg = "����־�ļ�����\n" + m_sLogFile;
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
	// ��0����Ϊϵͳ����
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

	GetPrivateProfileString("COMMON", "TITLE", "[*****]����", strTemp, STR_SIZE, strPath);
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

	// ȥ�����ġ�\��
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
	m_sErrorMsg = "ת�����";

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

	// �ȼ����ԴĿ¼��Ŀ��Ŀ¼�Ƿ����
	if(m_cSD.m_sSD.sSrcDir.IsEmpty() || _access(m_cSD.m_sSD.sSrcDir, 0 ) == -1)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("ԴĿ¼[%s]������", m_cSD.m_sSD.sSrcDir);
		return bRet;
	}
	if(m_cSD.m_sSD.sDesDir.IsEmpty() || _access(m_cSD.m_sSD.sDesDir, 0 ) == -1)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("Ŀ��Ŀ¼[%s]������", m_cSD.m_sSD.sDesDir);
		return bRet;
	}
	if(m_cSD.m_sSD.sSrcDir.CompareNoCase(m_cSD.m_sSD.sDesDir) == 0)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("ԴĿ¼������Ŀ��Ŀ¼��ͬ��");
		return bRet;
	}

	DWORD dwStart = GetTickCount();

	// ��������
	WriteLog("��������");
	m_nConvCount = LoadData(&m_cSD.m_dir, m_cSD.m_sSD.sSrcDir, m_cSD.m_sSD.sDesDir);

	// ִ������
	m_nConvNum = 0;
	WriteLog("----��ʼת��");
	bRet = BuildData(&m_cSD.m_dir);
	WriteLog("----����ת��");

	m_nErrorCode = 0;
	m_sErrorMsg.Format("ת�����!\n�ܹ�[%d]���ļ����ɹ�ת��[%d]���ļ�����ʱ[%d]ms", m_nConvCount, m_nConvNum, GetTickCount()-dwStart);
	WriteLog(m_sErrorMsg);

	return bRet;
}

// �ݹ�����Դ�ļ�����
int CSplitBill::LoadData(S_DIR* sDir, CString sSrcDir, CString sDesDir)
{
	int nCount = 0;

	// Ŀ¼��ֵ
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

		// ��ǰĿ¼��Ŀ¼
		if (filename.CompareNoCase(".")==0 || filename.CompareNoCase("..")==0)
			continue; 

		// Ŀ¼
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
		else // �ļ�
		{
			sDir->lsFilename.push_back(filename);
			++nCount;
		}
	}

	_findclose(lf);

	return nCount;
}

// �ݹ�����Ŀ���ļ�����
bool CSplitBill::BuildData(const S_DIR* sDir)
{
	bool bRet = false;

	// ���Ŀ��Ŀ¼�������ڣ��򴴽�
	if( (_access(sDir->sdesDirName, 0 )) != 0)
	{
		// ������
		if(FALSE == CreateDirectory(sDir->sdesDirName, NULL))
		{
			m_nErrorCode = GetLastError();
			return bRet;
		}
	}

	// ��ʼѭ��ת���ļ�
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

	// д��־
	CString sMsg;
	sMsg.Format("ת���ļ�[%s]->[%s]", srcFile, desFile);
	WriteLog(sMsg);
	m_pMainDlg->SetProcessInfo(sMsg);
	m_pMainDlg->SetPercentInfo((++m_nConvNum * 100) / m_nConvCount);

	// ���Դ�ļ�
	if( (_access(srcFile, 0 )) == -1)
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("Դ�ļ�[%s]������", srcFile);
		WriteLog("��������" + m_sErrorMsg);
		return bRet;
	}

	// Ŀ���ļ�������ڣ���ɾ��
	if( (_access(desFile, 0 )) == 0)
	{
		if(!DeleteFile(desFile))
		{
			m_nErrorCode = 0;
			m_sErrorMsg.Format("ɾ��Ŀ���ļ�[%s]ʧ��", desFile);
			WriteLog("��������" + m_sErrorMsg);
			return bRet;
		}
	}

	// ��Դ�ļ���ֻ����
	FILE* pFile;
	if (NULL == (pFile = ::fopen(srcFile, "r")))
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("txt�ļ���ʧ��:%s", srcFile);
		WriteLog("��������" + m_sErrorMsg);
		return bRet;
	}

	// ��Ŀ���ļ�
	FILE* pDesFile;
	if (NULL == (pDesFile = ::fopen(desFile, "a+")))
	{
		m_nErrorCode = 0;
		m_sErrorMsg.Format("txt�ļ���ʧ��:%s", desFile);
		WriteLog("��������" + m_sErrorMsg);

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