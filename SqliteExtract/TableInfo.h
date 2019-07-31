#pragma once

#include <string>
#include <list>
#include <map>

class CTableInfo
{
private:
	typedef struct __ColumnInfo
	{
		bool bColumnKey;
		bool bAutoKey;
		std::string sColumnName;
		std::string sColumnType;
		std::string sValueName;
		std::string sValueType;
		std::string sFormat;
	} COLUMN_INFO, *PCOLUMN_INFO;
	typedef std::list<COLUMN_INFO> CColumnInfoList;

	static std::string ConvertString( const std::string & sSrc, bool toTableStr = false );
	std::string GetValType( const std::string & sSqlType );
	bool IsAutoKey( std::string sCreateSql, std::string sColumnName );

public:
	CTableInfo( const std::string & sFilePath, const std::string & sTableName );
	~CTableInfo(void);

	bool BuildTableCode();

private:
	bool BuildH();
	bool BuildCPP();
	bool WriteCodeFile( std::string sFileName, std::string sBuf );

private:
	std::string m_sTableName;
	std::string m_sClassName;
	std::string m_sCreateSql;
	CColumnInfoList m_ColumnInfoList;
};

