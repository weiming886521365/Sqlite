#include "StdAfx.h"
#include "TableInfo.h"

#include <TSqlOperate.h>
#include <StringUtil.h>

#define VS_INT         "int"
#define VS_STRING      "std::string"

#define INT_DEFAULT    "0"
#define STRING_DEFAULT "\"\""


CTableInfo::CTableInfo( const std::string & sFilePath, const std::string & sTableName )
{
	std::string sCreateSql;

	m_sTableName = sTableName;
	m_sClassName = "C" + CTableInfo::ConvertString( sTableName );

	
	std::string sGetCreateSQl = SU::CStringUtil::StringFormat( "select sql from sqlite_master where type='table' and name='%s';", sTableName.c_str() );
	std::string sGetColumnSQl = SU::CStringUtil::StringFormat( "pragma table_info('%s');", sTableName.c_str() );

	std::list<RowInfo> TableSqlList;
	std::list<RowInfo> ColumnInfoList;
	SqlUtil::TSqlOperate<> SqlOperate( sFilePath );
	SqlOperate.LookupData( sGetCreateSQl, TableSqlList );
	sCreateSql = TableSqlList.size() != 0 ? TableSqlList.begin()->at("sql") : "";
	SqlOperate.LookupData( sGetColumnSQl, ColumnInfoList );
	for ( std::list<RowInfo>::iterator it = ColumnInfoList.begin(); it != ColumnInfoList.end(); it++ )
	{
		COLUMN_INFO CI;
		CI.sColumnName = it->at("name");
		CI.sColumnType =it->at( "type" );
		CI.bColumnKey = it->at( "pk" ) == "1";
		CI.bAutoKey = CI.bColumnKey ? IsAutoKey( sCreateSql, CI.sColumnName ) : false;
		CI.sValueType = GetValType( CI.sColumnType );
		CI.sValueName = (( 0 == CI.sValueType.compare("int") ) ? "n" : "s") + CTableInfo::ConvertString( CI.sColumnName );
		CI.sFormat = ( 0 == CI.sValueType.compare("int") ) ? "%d" : "\\\'%s\\\'";

		m_ColumnInfoList.push_back( CI );
	}

	for ( int i = 0; i < sCreateSql.size(); i++ )
	{
		if ( '\r' == sCreateSql.at(i) ) continue;
		if ( '\n' == sCreateSql.at(i) )
		{
			m_sCreateSql += "\\r";
			m_sCreateSql += "\\n";
			m_sCreateSql += '\\';
			m_sCreateSql += '\r';
		}
		m_sCreateSql += sCreateSql.at(i);
	}
	if ( m_sCreateSql.at( m_sCreateSql.length() - 1 ) == '\\' )
	{
		m_sCreateSql = m_sCreateSql.substr( 0, m_sCreateSql.length() - 2 );
	}
}


CTableInfo::~CTableInfo(void)
{
}

std::string CTableInfo::ConvertString( const std::string & sSrc, bool toTableStr )
{
	std::string sDest;
	bool bSlicer = true;

	if ( toTableStr )
	{
	}
	else
	{
		for ( int i = 0; i < sSrc.size(); i++ )
		{
			if ( sSrc.at( i ) == '_' )
			{
				bSlicer = true;
				continue;
			}

			if ( bSlicer && sSrc.at( i ) > 'a' && sSrc.at( i ) < 'z' )
			{
				sDest += ( sSrc.at( i ) + 'A' - 'a' );
			}
			else
			{
				sDest += ( sSrc.at( i ) );
			}

			bSlicer = false;
		}
	}

	return sDest;
}

std::string CTableInfo::GetValType( const std::string & sSqlType )
{
	std::string sValType;

	if ( std::string::npos != sSqlType.find( "INT" ) )
	{
		sValType = VS_INT;
	}
	else
	{
		sValType = VS_STRING;
	}

	return sValType;
}

bool CTableInfo::IsAutoKey( std::string sCreateSql, std::string sColumnName )
{
	bool bAutoKey = false;
	std::vector<std::string> vec;
	SU::CStringUtil::StringSplit( sCreateSql, ',', vec );
	
	for ( int i = 0; i < vec.size(); i++ )
	{
		if ( std::string::npos != vec[i].find(sColumnName) &&
			 std::string::npos != vec[i].find("AUTO") )
		{
			bAutoKey = true;
		}
	}

	return bAutoKey;
}

bool CTableInfo::BuildTableCode()
{
	return BuildH() && BuildCPP();
}

bool CTableInfo::BuildH()
{
	std::string sFullCode;

	//±àÐ´ÎÄ¼þÍ·
	sFullCode = SU::CStringUtil::StringFormat( "\
#pragma once\r\n\
\r\n\
#include <string.h>\r\n\
#include <TSqlOperate.h>\r\n\
\r\n\
class %s : public SqlUtil::ITableInfo\r\n\
{\r\n\
public:\r\n\
\t%s(void);\r\n\
\t~%s(void);\r\n\
\r\n\
public:\r\n\
\tvirtual std::string GetTableName() const;\r\n\
\tvirtual std::string GetCreateTableSql() const;\r\n\
\tvirtual std::string GetInsertSql() const;\r\n\
\tvirtual std::string GetRemoveSql() const;\r\n\
\tvirtual std::string GetUpdateSql() const;\r\n\
\tvirtual std::string GetLookupSql() const;\r\n\
\r\n\
\tvirtual void BuildTableInfo( std::map<std::string,std::string> TableInfoMap );\r\n\
\r\n\
public:\r\n",
	m_sClassName.c_str(),
	m_sClassName.c_str(),
	m_sClassName.c_str() );


	//±àÐ´³ÉÔ±±äÁ¿
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		sFullCode.append( SU::CStringUtil::StringFormat( "\t%s %s;\r\n", it->sValueType.c_str(), it->sValueName.c_str() ) );
	}


	//±àÐ´½áÎ²
	sFullCode.append("\r\n};\r\n");

	return WriteCodeFile( CTableInfo::ConvertString( m_sTableName ).append(".h"), sFullCode );;
}

bool CTableInfo::BuildCPP()
{
	std::string sFullCode;

	//±àÐ´ÎÄ¼þÍ·
	sFullCode = SU::CStringUtil::StringFormat( "\
#include \"stdafx.h\"\r\n\
#include \"%s.h\"\r\n\
#include <stdlib.h>\r\n\
#include <TableUtil.h>\r\n\
\r\n\r\n",
	CTableInfo::ConvertString( m_sTableName ).c_str() );

	//±àÐ´¹¹Ôìº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "%s::%s(void)\r\n\{\r\n", m_sClassName.c_str(), m_sClassName.c_str() ) );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		sFullCode.append( SU::CStringUtil::StringFormat( "\t%s=%s;\r\n", it->sValueName.c_str(), it->sValueType.compare(VS_INT) == 0 ? INT_DEFAULT : STRING_DEFAULT ) );
	}
	sFullCode.append("}\r\n\r\n");

	//±àÐ´Îö¹¹º¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "%s::~%s(void)\r\n{\r\n}\r\n\r\n", m_sClassName.c_str(), m_sClassName.c_str() ) );

	/************************************************************************/
	//±àÐ´GetTableNameº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
std::string %s::GetTableName() const \r\n\
{\r\n\
\treturn \"%s\";\r\n\
}\r\n\r\n",
m_sClassName.c_str(),
m_sTableName.c_str() ) );

	/************************************************************************/
	//±àÐ´GetCreateTableSqlº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
std::string %s::GetCreateTableSql() const \r\n\
{\r\n\
\treturn TU::CTableUtil::StringFormat( \"%s;\");\r\n\
}\r\n\r\n",
m_sClassName.c_str(),
m_sCreateSql.c_str() ) );

	/************************************************************************/
	//±àÐ´GetInsertSqlº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
std::string %s::GetInsertSql() const \r\n\
{\r\n\
\treturn TU::CTableUtil::StringFormat( \"insert into %s(",
m_sClassName.c_str(),
m_sTableName.c_str() ) );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey && it->bAutoKey )
		{
			continue;
		}
		sFullCode.append( SU::CStringUtil::StringFormat( " %s,", it->sColumnName.c_str() ) );
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(",") );
	sFullCode.append( ") values(" );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey && it->bAutoKey )
		{
			continue;
		}
		sFullCode.append( SU::CStringUtil::StringFormat( " %s,", it->sFormat.c_str() ) );
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(",") );
	sFullCode.append( ");\"," );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey && it->bAutoKey )
		{
			continue;
		}

		if ( 0 == it->sValueType.compare( VS_STRING ) )
		{
			sFullCode.append( SU::CStringUtil::StringFormat( " TU::CTableUtil::StringTransf( \%s ).c_str(),", it->sValueName.c_str() ) );
		}
		else
		{
			sFullCode.append( SU::CStringUtil::StringFormat( " %s,", it->sValueName.c_str() ) );
		}
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(",") );
	sFullCode.append( ");\r\n}\r\n\r\n" );

	/************************************************************************/
	//±àÐ´GetRemoveSqlº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
std::string %s::GetRemoveSql() const \r\n\
{\r\n\
\treturn TU::CTableUtil::StringFormat( \"delete from %s where",
m_sClassName.c_str(),
m_sTableName.c_str() ) );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey )
		{
			sFullCode.append( SU::CStringUtil::StringFormat( " %s=%s and", it->sColumnName.c_str(), it->sFormat.c_str() ) );
		}
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen("and") );
	sFullCode.append( ";\"," );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey )
		{
			if ( 0 == it->sValueType.compare( VS_STRING ) )
			{
				sFullCode.append( SU::CStringUtil::StringFormat( " TU::CTableUtil::StringTransf( \%s ).c_str(),", it->sValueName.c_str() ) );
			}
			else
			{
				sFullCode.append( SU::CStringUtil::StringFormat( " %s,", it->sValueName.c_str() ) );
			}
		}
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(",") );
	sFullCode.append( ");\r\n}\r\n\r\n" );

	/************************************************************************/
	//±àÐ´GetUpdateSqlº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
std::string %s::GetUpdateSql() const \r\n\
{\r\n\
\treturn TU::CTableUtil::StringFormat( \"update %s set ",
m_sClassName.c_str(),
m_sTableName.c_str() ) );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		sFullCode.append( SU::CStringUtil::StringFormat( " %s=%s,", it->sColumnName.c_str(), it->sFormat.c_str() ) );
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(",") );
	sFullCode.append( " where" );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey )
		{
			sFullCode.append( SU::CStringUtil::StringFormat( " %s=%s and", it->sColumnName.c_str(), it->sFormat.c_str() ) );
		}
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(" and") );
	sFullCode.append( ";\"," );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( 0 == it->sValueType.compare( VS_STRING ) )
		{
			sFullCode.append( SU::CStringUtil::StringFormat( " TU::CTableUtil::StringTransf( \%s ).c_str(),", it->sValueName.c_str() ) );
		}
		else
		{
			sFullCode.append( SU::CStringUtil::StringFormat( " %s,", it->sValueName.c_str() ) );
		}
	}
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( it->bColumnKey )
		{
			if ( 0 == it->sValueType.compare( VS_STRING ) )
			{
				sFullCode.append( SU::CStringUtil::StringFormat( " TU::CTableUtil::StringTransf( \%s ).c_str(),", it->sValueName.c_str() ) );
			}
			else
			{
				sFullCode.append( SU::CStringUtil::StringFormat( " %s,", it->sValueName.c_str() ) );
			}
		}
	}
	sFullCode = sFullCode.substr( 0, sFullCode.length() - strlen(",") );
	sFullCode.append( ");\r\n}\r\n\r\n" );

	/************************************************************************/
	//±àÐ´GetLookupSqlº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
std::string %s::GetLookupSql() const \r\n\
{\r\n\
\tstd::string sSQl = \"select * from %s where 1=1\";\r\n",
m_sClassName.c_str(),
m_sTableName.c_str() ) );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( 0 == it->sValueType.compare( VS_STRING ) )
		{
			sFullCode.append( SU::CStringUtil::StringFormat( "\
\tif ( %s != %s )\r\n\
\t{\r\n\
\t\tsSQl.append( TU::CTableUtil::StringFormat( \" and %s=\\\'%s\\\'\", \TU::CTableUtil::StringTransf( \%s ).c_str() ) );\r\n\
\t}\r\n\r\n",
STRING_DEFAULT,
it->sValueName.c_str(),
it->sColumnName.c_str(),
"%s",
it->sValueName.c_str() ) );
		}
		else
		{
			sFullCode.append( SU::CStringUtil::StringFormat( "\
\tif ( %s != %s )\r\n\
\t{\r\n\
\t\tsSQl.append( TU::CTableUtil::StringFormat( \" and %s=%s\", \%s ) );\r\n\
\t}\r\n\r\n",
INT_DEFAULT,
it->sValueName.c_str(),
it->sColumnName.c_str(),
"%d",
it->sValueName.c_str() ) );
		}
	}
	sFullCode.append( "\tsSQl.append(\";\");\r\n\treturn sSQl;\r\n}\r\n\r\n" );


	/************************************************************************/
	//±àÐ´BuildTableInfoº¯Êý
	sFullCode.append( SU::CStringUtil::StringFormat( "\
void %s::BuildTableInfo( std::map<std::string,std::string> TableInfoMap )\r\n\
{\r\n",
m_sClassName.c_str() ) );
	for (CColumnInfoList::const_iterator it = m_ColumnInfoList.begin(); it != m_ColumnInfoList.end(); it++ )
	{
		if ( 0 == it->sValueType.compare( VS_INT ) )
		{
			sFullCode.append( SU::CStringUtil::StringFormat( "\t%s=atoi(TableInfoMap[\"%s\"].c_str());\r\n", it->sValueName.c_str(), it->sColumnName.c_str() ) );
		}
		else
		{
			sFullCode.append( SU::CStringUtil::StringFormat( "\t%s=TableInfoMap[\"%s\"];\r\n", it->sValueName.c_str(), it->sColumnName.c_str() ) );
		}
	}
	sFullCode.append( "\r\n}\r\n\r\n" );

	return WriteCodeFile( CTableInfo::ConvertString( m_sTableName ).append(".cpp"), sFullCode );;
}

bool CTableInfo::WriteCodeFile( std::string sFileName, std::string sBuf )
{
	bool bSucc = false;

	HANDLE hFile = INVALID_HANDLE_VALUE; 
	DWORD dwBytesWritten = 0;

	do 
	{
		DeleteFile( sFileName.c_str() );
		hFile = CreateFile( sFileName.c_str(),                // name of the write
			GENERIC_WRITE,          // open for writing
			0,                      // do not share
			NULL,                   // default security
			CREATE_NEW,             // overwrite existing
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);                  // no attr. template

		if ( INVALID_HANDLE_VALUE == hFile )
		{
			break;
		}

		if( !WriteFile(hFile,       // open file handle
			sBuf.c_str(),           // start of data to write
			sBuf.size(),            // number of bytes to write
			&dwBytesWritten,        // number of bytes that were written
			NULL) )                 // no overlapped structure
		{
			break;
		}

		bSucc = true;
	} while ( false );

	if ( INVALID_HANDLE_VALUE != hFile )
	{
		CloseHandle( hFile );
	}

	return bSucc;
}