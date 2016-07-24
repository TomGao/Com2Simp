#include "stdafx.h"
#include "common.h"
#include <iostream>
#include <io.h>
#include "resource.h"

#pragma comment(lib, "Version.lib ") 

const int BUFFER_LEN = 10240;
#define		SAFERELEASE(p)			if((p)){delete[] (p);(p) = NULL;}

// 取得后缀名
CString GetFileExtName(CString filename)
{
	int nPos = filename.ReverseFind('.');

	if(nPos > 0)
	{
		return filename.Mid(nPos + 1);
	}

	return filename;
}

// 取得纯文件名
CString GetFileSingleName(CString filename)
{
	int nPos = filename.ReverseFind('.');

	if(nPos > 0)
	{
		return filename.Left(nPos);
	}

	return filename;
}

// 从文件路径中取得文件名
CString GetFileName(CString filename)
{
	int nPos = filename.ReverseFind('\\');

	if(nPos > 0)
	{
		return filename.Mid(nPos + 1);
	}

	return filename;
}

// 从文件路径中取得目录
CString GetFileDir(CString filename)
{
	int nPos = filename.ReverseFind('\\');

	if(nPos > 0)
	{
		return filename.Left(nPos);
	}

	return filename;
}

// 取得当前工程资源中的版本
CString GetVerInfo(void)
{
	CString   strVersion; 

	TCHAR   szFullPath[MAX_PATH]; 
	DWORD   dwVerInfoSize   =   0; 
	DWORD   dwVerHnd; 
	LPVOID  pFileInfo = NULL;

	GetModuleFileName(NULL,   szFullPath,   sizeof(szFullPath)); 
	dwVerInfoSize   =   GetFileVersionInfoSize(szFullPath,   &dwVerHnd); 
	if   (dwVerInfoSize) 
	{ 
		//   If   we   were   able   to   get   the   information,   process   it: 
		HANDLE     hMem; 
		LPVOID     lpvMem; 
		unsigned   int   uInfoSize   =   0; 

		hMem   =   GlobalAlloc(GMEM_MOVEABLE,   dwVerInfoSize); 
		lpvMem   =   GlobalLock(hMem); 
		GetFileVersionInfo(szFullPath,   dwVerHnd,   dwVerInfoSize,   lpvMem); 

		BOOL fRet = ::VerQueryValue(lpvMem,   (LPTSTR)"\\StringFileInfo\\040904b0\\ProductVersion",   &pFileInfo,   &uInfoSize); 

		char   szProductVer[20] = "";
		if (fRet && pFileInfo && uInfoSize)
			strcpy_s(szProductVer, (char *)pFileInfo);

		for (unsigned int i=0; i<strlen(szProductVer); i++)
			if (szProductVer[i]==',') szProductVer[i]='.';

		strVersion = CString(szProductVer); 

		GlobalUnlock(hMem); 
		GlobalFree(hMem); 
	}

	return strVersion;
}

// 拆分字符串
int SplitString(const CString& str, LS_STRING& outRet, CString split)
{
	outRet.clear();

	int lPos = 0;
	int rPos = 0;
	int len = 0;

	// 002,021,098
	while(rPos >= 0)
	{
		rPos = str.Find(split, lPos);

		// 如果已经结束了
		if(rPos < 0)
		{
			len = str.GetLength() - lPos;
		}
		else
		{
			len = rPos - lPos;
		}

		CString sTemp = str.Mid(lPos, len);
		sTemp.Trim();

		if (!sTemp.IsEmpty())
		{
			outRet.push_back(sTemp);
		}

		lPos = rPos + 1;
	}

	return (int)outRet.size();
}

// 删除目录
BOOL DeleteDirectory(CString psDirName) 
{ 
	SHFILEOPSTRUCT FileOp;
	ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = psDirName;
	FileOp.pTo = NULL;
	FileOp.wFunc = FO_DELETE;

	if(0 == SHFileOperation(&FileOp))
		return TRUE; 
	else
		return FALSE;
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)  
{
	if(uMsg == BFFM_INITIALIZED)
	{  
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;  
}

// 浏览文件夹
bool BrowseDir(CString title, CString &dir)
{
	bool bRet = false;

	BROWSEINFO bi; 
	bi.hwndOwner=NULL; 
	bi.pidlRoot=NULL; 
	bi.pszDisplayName=NULL; 
	bi.lpszTitle=title; 
	bi.iImage=IDR_MAINFRAME;
	bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE; 

	if( (_access(dir, 0 )) == -1)
	{
		bi.lParam =0;
		bi.lpfn = NULL;
	}
	else
	{
		bi.lParam = (long)(dir.GetBuffer(dir.GetLength()));//初始化路径，形如(_T("c:\\Symbian"));
		bi.lpfn = BrowseCallbackProc;
	}

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi); 

	// 放弃，则不做处理
	TCHAR path[255] = _T(""); 
	if ( !SHGetPathFromIDList(pIDList,path) ) 
	{
		dir.Empty();
		return false; 
	} 

	// free memory used     
	IMalloc * imalloc = 0;
	if ( SUCCEEDED(SHGetMalloc( &imalloc)))
	{
		imalloc->Free (pIDList);
		imalloc->Release();
	}

	CString s = path ; 
	if ( s.Right( 1 ) != _T("\\") ) 
		s+= _T("\\") ; 

	dir = s;

	return true;
} 

// 取得目录下类似的文件
bool GetFilesInDir(CString dir, CString sampleName, LS_STRING& outRet)
{
	bool bRet = false;
	outRet.clear();

	_finddata_t file;
	long lf;
	if((lf = _findfirst(dir + "*.*", &file))==-1l)
	{
		bRet = false;
	}
	else
	{
		while( _findnext( lf, &file ) == 0 )
		{
			CString filename(file.name);
			CString fileSingleName = GetFileSingleName(filename);

			if (fileSingleName.GetLength() < sampleName.GetLength())
				continue;

			// 只要取出一个
			if(fileSingleName.Find(sampleName) >= 0)
			{
				outRet.push_back(filename);
				bRet = true;
				continue;
			}
		}
	}

	_findclose(lf);

	return bRet;
}

// 取得路径下模板的文件名
bool GetFilenameByTemplate(CString sPath, CString sTemplate, LS_STRING& outRet)
{
	bool bRet = false;
	outRet.clear();

	CString fileSingleName = GetFileSingleName(sTemplate);

	int lPos = fileSingleName.ReverseFind('[');
	int rPos = fileSingleName.Find(']');

	CString sFilename;

	// [XXXXX]op_ccbd.mdd
	if (lPos == 0)
	{
		sFilename = fileSingleName.Mid(rPos+1);
	}
	// op_ccbd[XXXXX].mdd
	else if (rPos == fileSingleName.GetLength()-1)
	{
		sFilename = fileSingleName.Mid(0, lPos);
	}
	// [XXXXX]op_ccbd[XXXXX].mdd
	else if (lPos > rPos)
	{
		sFilename = fileSingleName.Mid(rPos+1, lPos-rPos-1);
	}
	else
	{
		// 不是模糊文件
		outRet.push_back(sTemplate);
		return true;
	}

	if (!sFilename.IsEmpty())
	{
		bRet = GetFilesInDir(sPath, sFilename, outRet);
	}

	return bRet;
}

// 读取FILE文件中的内容
int ReadLine(FILE* pFile, CString& strLine)
{
	char szBuff[BUFFER_LEN];
	int nLen;
	char * pos=NULL;
	do 
	{
		if (NULL == ::fgets(szBuff, BUFFER_LEN, pFile))
		{
			return -1;
		}
		if((pos=strstr(szBuff,"\n")))
			pos = '\0';
		if((pos=strstr(szBuff,"\r")))
			pos = '\0';
		if (0 < (nLen = (int)::strlen(szBuff)))
		{
			break;
		}

	} while (true);

	szBuff[nLen - 1] = '\0';

	strLine = szBuff;
	strLine.Trim();

	return 0;
}

// 简体转繁体
bool GB2GBK(char* data)
{
	ATLASSERT(data != NULL);

	//无内容，不用转换
	if(!strcmp(data, ""))
		return false;

	int dataLen = strlen(data);
	DWORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	int nReturn = LCMapStringA(wLCID, LCMAP_TRADITIONAL_CHINESE, data, dataLen, NULL, 0);

	if(!nReturn)
		return false;

	char *pcBuf = new char[nReturn + 1];

	__try
	{
		LCMapStringA(wLCID, LCMAP_TRADITIONAL_CHINESE, data, nReturn, pcBuf, nReturn + 1);
		strncpy(data, pcBuf, nReturn);
	}
	__finally
	{
		SAFERELEASE(pcBuf);
	}	

	return true;
}

// 繁体转简体
bool GBK2GB(char* data)
{
	ATLASSERT(data != NULL);

	if(!strcmp(data, ""))
		return false;

	int dataLen = strlen(data);

	DWORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_BIG5);
	int nReturn = LCMapStringA(wLCID, LCMAP_SIMPLIFIED_CHINESE, data, dataLen, NULL, 0);

	if(!nReturn)
		return false;

	char *pcBuf = new char[nReturn + 1];

	__try
	{
		LCMapStringA(wLCID, LCMAP_SIMPLIFIED_CHINESE, data, nReturn, pcBuf, nReturn + 1);
		strncpy(data, pcBuf, nReturn);
	}

	__finally
	{
		SAFERELEASE(pcBuf);
	}

	return true;
}

//// 向目标文件追加一行数据
//bool WriteLine(CString filename, CString& strLine)
//{
//	bool bRet = false;
//
//	FILE* pFile;
//
//	if (NULL == (pFile = ::fopen(filename, "a+")))
//	{
//		return bRet;
//	}
//
//	::fwrite(strLine, sizeof(char), strLine.GetLength(), pFile);
//	::fwrite("\n", sizeof(char), 1, pFile);
//
//	::fclose(pFile);
//
//	return true;
//}

//// GBK => Big5
//void GBK2BIG5(char *szBuf)
//{
//	if(!strcmp(szBuf, ""))
//		return ;
//
//	int nStrLen = strlen(szBuf);
//	wchar_t *pws = new wchar_t[nStrLen + 1];
//
//	MultiByteToWideChar(936, 0, szBuf, nStrLen, pws, nStrLen + 1);
//	BOOL bValue = false;
//	WideCharToMultiByte(950, 0, pws, nStrLen, szBuf, nStrLen + 1, "?", &bValue);
//	szBuf[nStrLen] = 0;
//
//	delete[] pws;
//}
//
//// Big5 => GBK
//void BIG52GBK(char *szBuf)
//{
//	if(!strcmp(szBuf, ""))
//		return;
//
//	int nStrLen = strlen(szBuf);
//	wchar_t *pws = new wchar_t[nStrLen + 1];
//
//	int nReturn = MultiByteToWideChar(950, 0, szBuf, nStrLen, pws, nStrLen + 1);
//	BOOL bValue = false;
//	nReturn = WideCharToMultiByte(936, 0, pws, nReturn, szBuf, nStrLen + 1, "?", &bValue);
//	szBuf[nReturn] = 0;
//
//	delete[] pws;
//}