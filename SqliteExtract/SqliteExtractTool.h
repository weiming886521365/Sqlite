#pragma once

#include <string>
#include <TableInfo.h>

namespace SET
{
class CSqliteExtractTool
{
public:
	static bool ExtractDb( const std::string & sFilePath );
	static bool ExtractTable( const std::string & sFilePath, const std::string & sTableName );
};

} // namespace SET
