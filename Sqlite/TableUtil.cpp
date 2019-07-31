#include "stdafx.h"
#include "TableUtil.h"
#include <shlwapi.h>
#include <stdarg.h>
#include <stdio.h>

#pragma comment(lib,"shlwapi.lib")

namespace TU
{

std::string CTableUtil::SqliteDbPath()
{
	CHAR szPath[ MAX_PATH ] = { 0 };
	GetModuleFileNameA( NULL, szPath, sizeof( szPath ) );
	PathRemoveFileSpecA( szPath );
	strcat( szPath, "\\SoftwareMgr.db" );
	return szPath;
}

std::string CTableUtil::StringFormat(const char * _Format, ...)
{
    std::string tmp;   
    va_list marker;
    
    va_start(marker, _Format);
    size_t num_of_chars = _vscprintf(_Format, marker);
    va_end(marker);
    
    

    if (num_of_chars > tmp.capacity()) {
        tmp.resize(num_of_chars + 1);
    }
    else
    {
        tmp.resize(tmp.capacity());
    }
    
    va_start(marker, _Format);
    vsnprintf((char *)tmp.data(), tmp.capacity(), _Format, marker);
    va_end(marker);

    return tmp.c_str();
}

std::string CTableUtil::StringTransf( const std::string & sVal )
{
	std::string sBuf;

	for ( int i = 0; i < sVal.size(); i++ )
	{
		if ( '\'' == sVal.at(i) )
		{
			sBuf += '\'';
		}
		sBuf += sVal.at(i);
	}

	return sBuf;
}

} // namespace TU
