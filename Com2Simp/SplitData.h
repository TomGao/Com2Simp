#ifndef _KSSPLITDATA
#define _KSSPLITDATA

#include "common.h"

// �ļ��ж���
struct S_DIR
{
	// Դ�ļ�������
	CString			ssrcDirName;
	// Ŀ���ļ�������
	CString			sdesDirName;
	// �ļ�����
	list<CString>	lsFilename;
	// �ļ��м���
	list<S_DIR>		lsDir;

	void Clear()
	{
		lsFilename.clear();
		lsDir.clear();
	}
};

// ת������
enum CONV_TYPE
{
	// ��ת��
	CT_C2S = 0,

	// ��ת��
	CT_S2C,
};

// ��������ݽṹ
struct SPLITDATA
{
	CString sSrcDir;
	CString sDesDir;

	// ת������
	CONV_TYPE	eConvType;
	// �Ƿ�ת��Ŀ¼(0-no, 1-yes)
	int nConvDir;
};

class CSplitData
{
public:
	CSplitData(void);
public:
	~CSplitData(void);

public:
	void ClearData();

public:
	// ���������
	SPLITDATA m_sSD;

	// ��ǰת����Ŀ¼����
	S_DIR m_dir;
};

#endif  