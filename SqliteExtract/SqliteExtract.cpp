// SqliteExtract.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SqliteExtract.h"

#include <shlwapi.h>
#include <SqliteExtractTool.h>

#pragma comment( lib, "shlwapi.lib" )

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	char szWorkPath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szWorkPath, MAX_PATH);
	PathRemoveFileSpecA(szWorkPath);
	SetCurrentDirectoryA( szWorkPath );
	SET::CSqliteExtractTool::ExtractDb( "SqliteExtract.db" );
}