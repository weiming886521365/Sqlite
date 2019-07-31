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
		ExeSqlSucc,      //�����ɹ�
		ParamInvalid,    //������Ч
		OpenDbFail,      //�����ݿ�ʧ��
		CreateTableFail, //������ʧ��
		ExeSqlFail,      //ִ��SQLʧ��
		UnknownErr,      //δ֪����
	};

	enum SqlType{
		InsertSql, //����
		RemoveSql, //ɾ��
		UpdateSql, //�޸�
		LookupSql  //��ѯ
	};

	class ITableInfo
	{
	public:
		ITableInfo(){};
		virtual ~ITableInfo(){};
		ITableInfo( const ITableInfo& other ){};

		virtual std::string GetTableName() const = 0;      //��ȡ����
		virtual std::string GetCreateTableSql() const = 0; //��ȡ������SQL���
		virtual std::string GetInsertSql() const = 0;      //��ȡ����SQL���
		virtual std::string GetRemoveSql() const = 0;      //��ȡɾ��SQL���
		virtual std::string GetUpdateSql() const = 0;      //��ȡ�޸�SQL���
		virtual std::string GetLookupSql() const = 0;      //��ȡ��ѯSQL���

		virtual void BuildTableInfo(std::map<std::string,std::string> TableInfoMap ) = 0; //��װ
	};

	// Ĭ��ģ�����
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

		//��������
		int InsertData( TableInfo & val );
		int InsertData( const std::list<TableInfo> & valList );

		//��������
		int LookupData( TableInfo & val, std::list<TableInfo> & valList );

		//�޸�����
		int UpdateData( TableInfo & val );
		int UpdateData( const std::list<TableInfo> & valList );

		//ɾ������
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
