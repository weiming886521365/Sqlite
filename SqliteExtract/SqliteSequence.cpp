#pragma once

#include <string.h>
#include <TSqlOperate.h>

class CSqliteSequence : public SqlUtil::ITableInfo
{
public:
	CSqliteSequence(void);
	~CSqliteSequence(void);

public:
	virtual std::string GetTableName() const;
	virtual std::string GetCreateTableSql() const;
	virtual std::string GetInsertSql() const;
	virtual std::string GetRemoveSql() const;
	virtual std::string GetUpdateSql() const;
	virtual std::string GetLookupSql() const;

	virtual void BuildTableInfo( std::map<std::string,std::string> TableInfoMap );

public:
	std::string sName;
	std::string sSeq;

};
#include "stdafx.h"
#include "SqliteSequence.h"
#include <stdlib.h>
#include <TableUtil.h>


CSqliteSequence::CSqliteSequence(void)
{
	sName="";
	sSeq="";
}

~CSqliteSequence::CSqliteSequence(void)
{
}

std::string CSqliteSequence::GetTableName() const 
{
	return "sqlite_sequence";
}

std::string CSqliteSequence::GetCreateTableSql() const 
{
	return TU::CTableUtil::StringFormat( "CREATE TABLE sqlite_sequence(name,seq);");
}

std::string CSqliteSequence::GetInsertSql() const 
{
	return TU::CTableUtil::StringFormat( "insert into sqlite_sequence( name, seq) values( \"%s\", \"%s\");" sName.c_str(), sSeq.c_str());
}

std::string CSqliteSequence::GetRemoveSql() const 
{
	return TU::CTableUtil::StringFormat( "delete from sqlite_sequence wh;);
}

std::string CSqliteSequence::GetUpdateSql() const 
{
	return TU::CTableUtil::StringFormat( "update sqlite_sequence set  name=\"%s\", seq=\"%s\" w;", sName.c_str(), sSeq.c_str());
}

std::string CSqliteSequence::GetLookupSql() const 
{
	return TU::CTableUtil::StringFormat( "select * from TFS_apprflowinfo;");
}

std::string CSqliteSequence::BuildTableInfo( std::map<std::string,std::string> TableInfoMap )
{
	sName=TableInfoMap["name"];
	sSeq=TableInfoMap["seq"];

}

