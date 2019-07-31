#pragma once
#include <string>
#include <vector>

namespace SU
{

class CStringUtil
{
public:
	static std::string StringFormat( const char * _Format, ...);
	static void StringSplit( std::string sSrc, char splitchar, std::vector<std::string>& vec );
};

} // namespace SU