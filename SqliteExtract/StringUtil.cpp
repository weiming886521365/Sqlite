#include "stdafx.h"
#include "StringUtil.h"
#include <stdarg.h>
#include <stdio.h>

namespace SU
{

std::string CStringUtil::StringFormat(const char * _Format, ...)
{
    std::string tmp;   
    va_list marker;
    
    va_start(marker, _Format);
#ifdef WIN32
    size_t num_of_chars = _vscprintf(_Format, marker);
#else
    size_t num_of_chars = vprintf(_Format, marker);
#endif
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

void CStringUtil::StringSplit( std::string sSrc, char splitchar, std::vector<std::string> & vec )
{  
	vec.clear();
	int start = 0;
	int length = sSrc.length();
	for ( int i = 0; i < length; i++ )
	{
		if( sSrc[i] == splitchar && i == 0 )//第一个就遇到分割符  
		{  
			start += 1;  
		}  
		else if( sSrc[i] == splitchar )  
		{  
			vec.push_back( sSrc.substr( start, i - start ) );  
			start = i+1;  
		}  
		else if( i == length-1 )//到达尾部  
		{  
			vec.push_back( sSrc.substr( start, i+1 - start ) );  
		}  
	}
}

} // namespace SU
