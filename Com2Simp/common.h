#ifndef _KSSPLITBILLTOOL_COMMON
#define _KSSPLITBILLTOOL_COMMON

#include <list>
#include <vector>
using namespace std;

typedef list<CString> LS_STRING;

// 取得后缀名
CString GetFileExtName(CString filename);

// 取得纯文件名
CString GetFileSingleName(CString filename);

// 从文件路径中取得文件名
CString GetFilename(CString filename);

// 从文件路径中取得目录
CString GetFileDir(CString filename);

// 取得当前工程资源中的版本
CString GetVerInfo(void);

// 拆分字符串
int SplitString(const CString& str, LS_STRING& outRet, CString split = ",");

// 删除目录
BOOL DeleteDirectory(CString psDirName);

// 浏览文件夹
bool BrowseDir(CString title, CString &dir);

// 取得目录下类似的文件
bool GetFilesInDir(CString dir, CString sampleName, LS_STRING& outRet);

// 取得路径下模板的文件名
bool GetFilenameByTemplate(CString sPath, CString sTemplate, LS_STRING& outRet);

// 读取FILE文件中的内容
int ReadLine(FILE* pFile, CString& strLine);

// 向目标文件追加一行数据
bool WriteLine(CString filename, CString& strLine);

// GBK => Big5
void GBK2BIG5(char *szBuf);

// Big5 => GBK
void BIG52GBK(char *szBuf);

// 简体转繁体
bool GB2GBK(char* data);

// 繁体转简体
bool GBK2GB(char* data);

#endif  