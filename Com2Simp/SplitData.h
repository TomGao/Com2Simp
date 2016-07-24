#ifndef _KSSPLITDATA
#define _KSSPLITDATA

#include "common.h"

// 文件夹定义
struct S_DIR
{
	// 源文件夹名称
	CString			ssrcDirName;
	// 目标文件夹名称
	CString			sdesDirName;
	// 文件集合
	list<CString>	lsFilename;
	// 文件夹集合
	list<S_DIR>		lsDir;

	void Clear()
	{
		lsFilename.clear();
		lsDir.clear();
	}
};

// 转换类型
enum CONV_TYPE
{
	// 繁转简
	CT_C2S = 0,

	// 简转繁
	CT_S2C,
};

// 保存的数据结构
struct SPLITDATA
{
	CString sSrcDir;
	CString sDesDir;

	// 转换类型
	CONV_TYPE	eConvType;
	// 是否转换目录(0-no, 1-yes)
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
	// 保存的数据
	SPLITDATA m_sSD;

	// 当前转换的目录对象
	S_DIR m_dir;
};

#endif  