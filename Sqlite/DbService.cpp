#include "stdafx.h"
#include "DbService.h"
#include <stdio.h>
#include <sqlite3.h>

namespace DbUtil
{

int CDbService::IsDatabaseExist( const std::string & strDbPath )
{
	//return CFileHelper::FileFind( strDbPath.c_str() ) ? DB_OK : DB_FAIL;
	return DB_OK;
}

int CDbService::OpenDatabase( const std::string & strDbPath, void* & pDb )
{
	int nResult = DB_FAIL;
	sqlite3 * dbptr = NULL;
	if( SQLITE_OK == sqlite3_open( strDbPath.c_str(), &dbptr ) )
	{
		pDb = ( void* )dbptr;
		nResult = DB_OK;
	}
	else
	{
		sqlite3_close( dbptr );
	}
	return nResult;
}

int CDbService::CreateDatabase( const std::string & strDbPath, void* & pDb )
{
	return OpenDatabase( strDbPath, pDb );
}

int CDbService::CloseDatabase( void* pDb )
{
	return ( SQLITE_OK == sqlite3_close( (sqlite3 *)pDb ) ) ? DB_OK : DB_FAIL;
}

int CDbService::DeleteDatabase( const std::string & strDbPath )
{
	//return CFileHelper::FileDelete( strDbPath.c_str() ) ? DB_OK : DB_FAIL;
	return DB_OK;
}

int CDbService::IsTableExist( void* pDb, const std::string & strTableName )
{
	int nResult = DB_FAIL;
	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	char szCmd[ 128 ] = { 0 };
	sprintf( szCmd, "select 0 from %s;", strTableName.c_str() );
	if( SQLITE_OK == sqlite3_exec( dbptr, szCmd, NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}
	return nResult;
}

int CDbService::CreateTable( void* pDb, const std::string & strTableName, const std::string & strSql )
{
	int nResult = DB_FAIL;

	if( DB_OK == IsTableExist( pDb, strTableName ) ) return DB_OK;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, strSql.c_str(), NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::DeleteTable( void* pDb, const std::string & strTableName )
{
	int nResult = DB_FAIL;
	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	char szCmd[ 128 ] = { 0 };
	sprintf( szCmd, "delete %s", strTableName.c_str() );
	if( SQLITE_OK == sqlite3_exec( dbptr, szCmd, NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::InsertData( void* pDb, const std::string & strSql )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, strSql.c_str(), NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::FindData( void* pDb, const std::string & strSql )
{
	int nResult = DB_FAIL;

	int nRow = 0;
	int nCol = 0;
	char ** pazResult = NULL;
	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_get_table( dbptr, strSql.c_str(), &pazResult, &nRow, &nCol, &errmsg ) )
	{
		if( nRow > 0 && nCol > 0 )
		{
			nResult = DB_OK;
		}
		sqlite3_free_table( pazResult );
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::LookupData( void* pDb, const std::string & strSql, IDbCallback * pCallback )
{
	int nResult = DB_FAIL;

	int nRow = 0;
	int nCol = 0;
	char ** pazResult = NULL;
	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_get_table( dbptr, strSql.c_str(), &pazResult, &nRow, &nCol, &errmsg ) )
	{
		if( nRow > 0 && nCol > 0 )
		{
			if( pCallback != NULL ) pCallback->OnLookupData( nRow, nCol, pazResult );
			nResult = DB_OK;
		}
		sqlite3_free_table( pazResult );
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::RemoveData( void* pDb, const std::string & strSql )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, strSql.c_str(), NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::UpdateData( void* pDb, const std::string & strSql )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, strSql.c_str(), NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::ExecSql( void* pDb, const std::string & strSql )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, strSql.c_str(), NULL, NULL, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::BeginTrans( void* pDb )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, "BEGIN", 0, 0, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::AddTrans( void* pDb, const std::string & strSql )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, strSql.c_str(), 0, 0, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

int CDbService::EndTrans( void* pDb )
{
	int nResult = DB_FAIL;

	char * errmsg = NULL;
	sqlite3 * dbptr = static_cast<sqlite3 *>(pDb);
	if( SQLITE_OK == sqlite3_exec( dbptr, "COMMIT", 0, 0, &errmsg ) )
	{
		nResult = DB_OK;
	}

	if( NULL != errmsg )
	{
		sqlite3_free( errmsg );
	}

	return nResult;
}

} // namespace DbUtil
