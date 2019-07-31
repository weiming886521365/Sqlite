#include "stdafx.h"
#include "LookupDataCallBack.h"

#include <string>
#include <vector>
#include <list>
#include <map>

CSelectCallBack::CSelectCallBack(void)
{
}


CSelectCallBack::~CSelectCallBack(void)
{
}


int CSelectCallBack::OnLookupData( int nRow, int nCol, char ** pData )
{
	std::vector<std::string> TableHead;
	if ( nRow > 0 )
	{
		//获取表头
		for ( int i = 0; i < nCol; i++ )
		{
			TableHead.push_back(pData[ TABLE_HEAD_INDEX * nCol + i ] == NULL ? "" : pData[ TABLE_HEAD_INDEX * nCol + i ]);
		}

		//获取查询结果
		for( int i = 1; i <= nRow; ++i )
		{
			RowInfo RI;
			for ( int j = 0; j < nCol; j++ )
			{
				RI[ TableHead[ j ] ] = pData[ i * nCol + j ] == NULL ? "" : pData[ i * nCol + j ];
			}
			m_ResultInfo.push_back(RI);
		}
	}

	return 0;
}