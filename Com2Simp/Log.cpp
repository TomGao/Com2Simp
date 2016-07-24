#include "StdAfx.h"
#include "Log.h"

CLog::CLog(void)
{
	m_pFile = NULL;
}

CLog::~CLog(void)
{
	if (m_pFile != NULL)
	{
		::fclose(m_pFile);
	}
}

bool CLog::Open(CString filename)
{
	if (NULL == (m_pFile = ::fopen(filename, "a+")))
	{
		return false;
	}

	return true;
}

void CLog::WriteLog(CString log)
{
	::fwrite(log, sizeof(char), log.GetLength(), m_pFile);
	::fwrite("\n", sizeof(char), 1, m_pFile);
}