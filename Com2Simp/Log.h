#ifndef _KSCLOG
#define _KSCLOG

class CLog
{
public:
	CLog(void);
public:
	~CLog(void);

public:
	bool Open(CString filename);

	void WriteLog(CString log);

private:
	FILE* m_pFile;
};

#endif