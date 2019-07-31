#pragma once

#include <string>
#include <list>
#include <map>

#include "DbService.h"

#define TABLE_HEAD_INDEX 0
typedef std::map<std::string,std::string> RowInfo;

class CSelectCallBack : public DbUtil::IDbCallback
{
public:
	CSelectCallBack(void);
	~CSelectCallBack(void);

	virtual int OnLookupData( int nRow, int nCol, char ** pData );

	std::list<RowInfo> GetResultInfo(){ return m_ResultInfo; }

private:
	std::list<RowInfo> m_ResultInfo;
};
