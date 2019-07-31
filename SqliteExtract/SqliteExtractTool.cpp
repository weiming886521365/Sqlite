#include "StdAfx.h"
#include "SqliteExtractTool.h"

#include <TSqlOperate.h>
#include <TableInfo.h>


namespace SET
{

bool CSqliteExtractTool::ExtractDb( const std::string & sFilePath )
{
	bool bSucc = true;

	std::list<RowInfo> TableList;
	SqlUtil::TSqlOperate<> SqlOperate( sFilePath );

	do 
	{
		if ( SqlUtil::ExeSqlSucc != SqlOperate.LookupData( "select name from sqlite_master where type='table' order by name;", TableList ) )
		{
			bSucc = false;
			break;
		}

		for ( std::list<RowInfo>::iterator it = TableList.begin(); bSucc && it != TableList.end(); it++ )
		{
			if ( it->at("name") == "sqlite_sequence" )
			{
				continue;
			}

			bSucc = CSqliteExtractTool::ExtractTable( sFilePath, it->at("name") );
		}
	} while ( false );

	return bSucc;
}

bool CSqliteExtractTool::ExtractTable( const std::string & sFilePath, const std::string & sTableName  )
{
	CTableInfo TableInfo( sFilePath, sTableName );
	return TableInfo.BuildTableCode();
}

} // namespace SET
