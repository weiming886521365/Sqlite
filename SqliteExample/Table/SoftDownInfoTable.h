#pragma once

#include <string.h>
#include <TSqlOperate.h>

class CSoftDownInfoTable : public SqlUtil::ITableInfo
{
public:
	CSoftDownInfoTable(void);
	~CSoftDownInfoTable(void);

public:
	virtual std::string GetTableName() const;
	virtual std::string GetCreateTableSql() const;
	virtual std::string GetInsertSql() const;
	virtual std::string GetRemoveSql() const;
	virtual std::string GetUpdateSql() const;
	virtual std::string GetLookupSql() const;

	virtual void BuildTableInfo( std::map<std::string,std::string> TableInfoMap );

public:
	int nSoftId;
	std::string sSoftName;
	std::string sVersions;
	std::string sSoftDownPath;
	std::string sSoftDownUrl;
	std::string sSoftLogoName;
	std::string sOnekeyQparam;
	std::string sFileMd5;
	int nFileSize;
	int nCurrentProgress;
	int nCurrentSpeed;
	int nRemainIme;
	int nTaskState;
	int nReserved1;
	int nReserved2;
	std::string sReserved3;
	std::string sReserved4;
	std::string sReserved5;

};
