#pragma once

#include <string>
#include <list>
#include <map>

#include <DbService.h>
#include <LookupDataCallBack.h>
#include <MutexLock.h>

namespace SqlUtil
{

	enum SqlResultCode{
		ExeSqlSucc,      //操作成功
		ParamInvalid,    //参数无效
		OpenDbFail,      //打开数据库失败
		CreateTableFail, //创建表失败
		ExeSqlFail,      //执行SQL失败
		UnknownErr,      //未知错误
	};

	enum SqlType{
		InsertSql, //插入
		RemoveSql, //删除
		UpdateSql, //修改
		LookupSql  //查询
	};

	class ITableInfo
	{
	public:
		ITableInfo(){};
		virtual ~ITableInfo(){};
		ITableInfo( const ITableInfo& other ){};

		virtual std::string GetTableName() const = 0;      //获取表名
		virtual std::string GetCreateTableSql() const = 0; //获取创建表SQL语句
		virtual std::string GetInsertSql() const = 0;      //获取插入SQL语句
		virtual std::string GetRemoveSql() const = 0;      //获取删除SQL语句
		virtual std::string GetUpdateSql() const = 0;      //获取修改SQL语句
		virtual std::string GetLookupSql() const = 0;      //获取查询SQL语句

		virtual void BuildTableInfo(std::map<std::string,std::string> TableInfoMap ) = 0; //组装
	};

	// 默认模板表类
	class CDefaultTableImpl : public ITableInfo
	{
	public:
		CDefaultTableImpl(){};
		virtual ~CDefaultTableImpl(){};
		CDefaultTableImpl( const CDefaultTableImpl& other ){};

		virtual std::string GetTableName() const { return ""; };
		virtual std::string GetCreateTableSql() const { return ""; };
		virtual std::string GetInsertSql() const { return ""; };
		virtual std::string GetRemoveSql() const { return ""; };
		virtual std::string GetUpdateSql() const { return ""; };
		virtual std::string GetLookupSql() const { return ""; };

		virtual void BuildTableInfo(std::map<std::string,std::string> TableInfoMap ){};
	};

	template < typename TableInfo = CDefaultTableImpl >
	class TSqlOperate{
	public:
		TSqlOperate( std::string sDbFilePath );
		~TSqlOperate();

		//插入数据
		int InsertData( TableInfo & val );
		int InsertData( const std::list<TableInfo> & valList );

		//查找数据
		int LookupData( TableInfo & val, std::list<TableInfo> & valList );

		//修改数据
		int UpdateData( TableInfo & val );
		int UpdateData( const std::list<TableInfo> & valList );

		//删除数据
		int RemoveData( TableInfo & val );
		int RemoveData( const std::list<TableInfo> & valList );

		//Sql
		int FindData( const std::string & sSql );
		int LookupData( const std::string & sSql, std::list<TableInfo> & valList );
		int LookupData( const std::string & sSql, std::list<RowInfo> & valList );
		int ExecSql( const std::string & sSql );

	private:
		int OpenTable( void* & pTable );
		int CloseTable( void* & pTable );

	private:
		std::string m_sDbFilePath;

		SYNC::CMutexLock* m_pML;
	};



	template < typename TableInfo >
	TSqlOperate<TableInfo>::TSqlOperate( std::string sDbFilePath ){

		m_sDbFilePath = sDbFilePath;
		std::string sDbFileName = m_sDbFilePath;
		int nPos = sDbFileName.rfind('\\');
		if( std::string::npos != nPos ) sDbFileName.at(nPos) = '/';
		nPos = sDbFileName.rfind('/');
		if( std::string::npos != nPos ) sDbFileName = sDbFileName.substr( nPos + 1 );
		m_pML = new SYNC::CMutexLock(sDbFileName);
	}

	template < typename TableInfo >
	TSqlOperate<TableInfo>::~TSqlOperate(){
		if ( NULL != m_pML )
		{
			delete m_pML;
			m_pML = NULL;
		}
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::InsertData( TableInfo & val )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::InsertData( pTable, val.GetInsertSql() ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::InsertData( const std::list<TableInfo> & valList )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( FALSE == SUCCEEDED( DbUtil::CDbService::BeginTrans( pTable ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			for ( typename std::list<TableInfo>::const_iterator it = valList.begin(); it != valList.end(); it++ )
			{
				if ( !DB_RESULT_SUCCEEDED( DbUtil::CDbService::AddTrans( pTable, it->GetInsertSql() ) ) )
				{
					nResultCode = ExeSqlFail;
					//break;
				}
			}

			if ( FALSE == SUCCEEDED( DbUtil::CDbService::EndTrans( pTable ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::LookupData( TableInfo & val, std::list<TableInfo> & valList )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;
		CSelectCallBack SelectCallBack;
		std::list<RowInfo> RowInfolist;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::LookupData( pTable, val.GetLookupSql(), &SelectCallBack ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			RowInfolist = SelectCallBack.GetResultInfo();
			for ( std::list<RowInfo>::iterator it = RowInfolist.begin(); it != RowInfolist.end(); it++ )
			{
				TableInfo TI;
				TI.BuildTableInfo( *it );
				valList.push_back( TI );
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::UpdateData( TableInfo & val )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::UpdateData( pTable, val.GetUpdateSql() ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::UpdateData( const std::list<TableInfo> & valList )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
			nResultCode = nTResultCode;
			break;
		}

		if ( FALSE == SUCCEEDED( DbUtil::CDbService::BeginTrans( pTable ) ) )
		{
			nResultCode = ExeSqlFail;
			break;
		}

        for ( typename std::list<TableInfo>::const_iterator it = valList.begin(); it != valList.end(); it++ )
		{
			if ( !DB_RESULT_SUCCEEDED( DbUtil::CDbService::AddTrans( pTable, it->GetUpdateSql() ) ) )
			{
				nResultCode = ExeSqlFail;
				//break;
			}
		}

		if ( FALSE == SUCCEEDED( DbUtil::CDbService::EndTrans( pTable ) ) )
		{
			nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::RemoveData( TableInfo & val )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::RemoveData( pTable, val.GetRemoveSql() ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::RemoveData( const std::list<TableInfo> & valList )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
		}

		if ( FALSE == SUCCEEDED( DbUtil::CDbService::BeginTrans( pTable ) ) )
		{
			nResultCode = ExeSqlFail;
			break;
		}

        for ( typename std::list<TableInfo>::const_iterator it = valList.begin(); it != valList.end(); it++ )
		{
			if ( !DB_RESULT_SUCCEEDED( DbUtil::CDbService::AddTrans( pTable, it->GetRemoveSql() ) ) )
			{
				nResultCode = ExeSqlFail;
				//break;
			}
		}

		if ( FALSE == SUCCEEDED( DbUtil::CDbService::EndTrans( pTable ) ) )
		{
			nResultCode = ExeSqlFail;
			break;
		}

		nResultCode = ExeSqlSucc;
	} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::FindData( const std::string & sSql )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::FindData( pTable, sSql ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::LookupData( const std::string & sSql, std::list<TableInfo> & valList )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;
		CSelectCallBack SelectCallBack;
		std::list<RowInfo> RowInfolist;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::LookupData( pTable, sSql, &SelectCallBack ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			RowInfolist = SelectCallBack.GetResultInfo();
			for ( typename std::list<RowInfo>::iterator it = RowInfolist.begin(); it != RowInfolist.end(); it++ )
			{
				TableInfo TI;
				TI.BuildTableInfo( *it );
				valList.push_back( TI );
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::LookupData( const std::string & sSql, std::list<RowInfo> & valList )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;
		CSelectCallBack SelectCallBack;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::LookupData( pTable, sSql, &SelectCallBack ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			valList = SelectCallBack.GetResultInfo();

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::ExecSql( const std::string & sSql )
	{
		int nResultCode       = UnknownErr;

		int nTResultCode      = UnknownErr;
		void* pTable          = NULL;

		SYNC::CMutexLock::Owner lock( *m_pML );

		do 
		{
			nTResultCode = OpenTable( pTable );
			if ( ExeSqlSucc != nTResultCode )
			{
				nResultCode = nTResultCode;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::ExecSql( pTable, sSql ) ) )
			{
				nResultCode = ExeSqlFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		CloseTable( pTable );

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::OpenTable( void* & pTable )
	{
		int nResultCode         = UnknownErr;

		TableInfo* pTableInfo   = NULL;
		ITableInfo* pITableInfo = NULL;

		do 
		{
			pTableInfo = new TableInfo;
			pITableInfo = dynamic_cast< ITableInfo* >( pTableInfo );
			if ( NULL == pITableInfo )
			{
				nResultCode = ParamInvalid;
				break;
			}

			if( !DB_RESULT_SUCCEEDED(DbUtil::CDbService::OpenDatabase( m_sDbFilePath, pTable ) ) )
			{
				nResultCode = OpenDbFail;
				break;
			}

			if ( !DB_RESULT_SUCCEEDED( DbUtil::CDbService::IsTableExist( pTable, pITableInfo->GetTableName() ) ) && 
				!DB_RESULT_SUCCEEDED( DbUtil::CDbService::CreateTable( pTable, pITableInfo->GetTableName(), pITableInfo->GetCreateTableSql() ) ) )
			{
				nResultCode = CreateTableFail;
				break;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		if ( NULL != pTableInfo )
		{
			delete pTableInfo;
		}

		return nResultCode;
	}

	template < typename TableInfo >
	int TSqlOperate<TableInfo>::CloseTable( void* & pTable )
	{
		int nResultCode = UnknownErr;

		do 
		{
			if ( NULL != pTable )
			{
				DbUtil::CDbService::CloseDatabase( pTable );
				pTable = NULL;
			}

			nResultCode = ExeSqlSucc;
		} while ( false );

		return nResultCode;
	}



} // namespace DbUtil
