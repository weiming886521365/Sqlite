// SqliteExample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SqliteExample.h"

#include <list>
#include <TSqlOperate.h>
#include <TableUtil.h>
#include <SoftDownInfoTable.h>

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	DeleteFileA( TU::CTableUtil::SqliteDbPath().c_str() );

	SqlUtil::TSqlOperate<CSoftDownInfoTable> SqlOperate( TU::CTableUtil::SqliteDbPath() );

	// ²åÈë
	{
		CSoftDownInfoTable SDT;
		SDT.nSoftId = 123;
		SDT.sSoftName = "Soft1";
		SDT.sVersions = "1.0.0.1";
		if ( SqlUtil::ExeSqlSucc != SqlOperate.InsertData( SDT ) )
		{
			return 1;
		}
	}

	// ÐÞ¸Ä
	{
		CSoftDownInfoTable SDT;
		SDT.nSoftId = 123;
		SDT.sSoftName = "Soft1";
		SDT.sVersions = "1.0.0.2";
		if ( SqlUtil::ExeSqlSucc != SqlOperate.UpdateData( SDT ) )
		{
			return 1;
		}
	}

	// ²éÑ¯
	{
		std::list<CSoftDownInfoTable> valList;
		CSoftDownInfoTable SDT;
		SDT.nSoftId = 123;
		if ( SqlUtil::ExeSqlSucc != SqlOperate.LookupData( SDT, valList ) )
		{
			return 1;
		}
	}

	// É¾³ý
	{
		CSoftDownInfoTable SDT;
		SDT.nSoftId = 123;
		if ( SqlUtil::ExeSqlSucc != SqlOperate.RemoveData( SDT ) )
		{
			return 1;
		}
	}

	return 0;
}