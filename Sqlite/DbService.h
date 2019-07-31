#pragma once

#include <string>

namespace DbUtil
{

enum DbResultCode{
	DB_OK = 0,
	DB_FAIL
};

#define DB_RESULT_SUCCEEDED(code) code == DbUtil::DB_OK

class IDbCallback
{
public:
	virtual ~IDbCallback(){};

public:
	//
	// 数据以二维字符指针矩阵形式返回
	// nRow：行总数
	// nCol：列总数
	//
	virtual int OnLookupData( int nRow, int nCol, char ** pData ) = 0;
};

class CDbService
{
public:
	static int IsDatabaseExist( const std::string & strDbPath );
	static int OpenDatabase( const std::string & strDbPath, void* & pDb );
	static int CreateDatabase( const std::string & strDbPath, void* & pDb );
	static int CloseDatabase( void* pDb );
	static int DeleteDatabase( const std::string & strDbPath );

	static int IsTableExist( void* pDb, const std::string & strTableName );
	static int CreateTable( void* pDb, const std::string & strTableName, const std::string & strSql );
	static int DeleteTable( void* pDb, const std::string & strTableName );

	static int InsertData( void* pDb, const std::string & strSql );
	static int FindData( void* pDb, const std::string & strSql );
	static int LookupData( void* pDb, const std::string & strSql, IDbCallback * pCallback );
	static int RemoveData( void* pDb, const std::string & strSql );
	static int UpdateData( void* pDb, const std::string & strSql );
	static int ExecSql( void* pDb, const std::string & strSql );

	static int BeginTrans( void* pDb );
	static int AddTrans( void* pDb, const std::string & strSql );
	static int EndTrans( void* pDb );
};


} // namespace DbUtil