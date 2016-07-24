#ifndef _KSSPLITBILLTOOL_COMMON
#define _KSSPLITBILLTOOL_COMMON

#include <list>
#include <vector>
using namespace std;

typedef list<CString> LS_STRING;

// ȡ�ú�׺��
CString GetFileExtName(CString filename);

// ȡ�ô��ļ���
CString GetFileSingleName(CString filename);

// ���ļ�·����ȡ���ļ���
CString GetFilename(CString filename);

// ���ļ�·����ȡ��Ŀ¼
CString GetFileDir(CString filename);

// ȡ�õ�ǰ������Դ�еİ汾
CString GetVerInfo(void);

// ����ַ���
int SplitString(const CString& str, LS_STRING& outRet, CString split = ",");

// ɾ��Ŀ¼
BOOL DeleteDirectory(CString psDirName);

// ����ļ���
bool BrowseDir(CString title, CString &dir);

// ȡ��Ŀ¼�����Ƶ��ļ�
bool GetFilesInDir(CString dir, CString sampleName, LS_STRING& outRet);

// ȡ��·����ģ����ļ���
bool GetFilenameByTemplate(CString sPath, CString sTemplate, LS_STRING& outRet);

// ��ȡFILE�ļ��е�����
int ReadLine(FILE* pFile, CString& strLine);

// ��Ŀ���ļ�׷��һ������
bool WriteLine(CString filename, CString& strLine);

// GBK => Big5
void GBK2BIG5(char *szBuf);

// Big5 => GBK
void BIG52GBK(char *szBuf);

// ����ת����
bool GB2GBK(char* data);

// ����ת����
bool GBK2GB(char* data);

#endif  