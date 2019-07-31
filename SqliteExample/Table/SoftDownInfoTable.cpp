#include "stdafx.h"
#include "SoftDownInfoTable.h"
#include <stdlib.h>
#include <TableUtil.h>


CSoftDownInfoTable::CSoftDownInfoTable(void)
{
	nSoftId=0;
	sSoftName="";
	sVersions="";
	sSoftDownPath="";
	sSoftDownUrl="";
	sSoftLogoName="";
	sOnekeyQparam="";
	sFileMd5="";
	nFileSize=0;
	nCurrentProgress=0;
	nCurrentSpeed=0;
	nRemainIme=0;
	nTaskState=0;
	nReserved1=0;
	nReserved2=0;
	sReserved3="";
	sReserved4="";
	sReserved5="";
}

CSoftDownInfoTable::~CSoftDownInfoTable(void)
{
}

std::string CSoftDownInfoTable::GetTableName() const 
{
	return "SoftDownInfoTable";
}

std::string CSoftDownInfoTable::GetCreateTableSql() const 
{
	return TU::CTableUtil::StringFormat( "CREATE TABLE SoftDownInfoTable ( \r\n\
    soft_id          INTEGER PRIMARY KEY,\r\n\
    soft_name        TEXT,\r\n\
    versions         TEXT,\r\n\
    soft_down_path   TEXT,\r\n\
    soft_down_url    TEXT,\r\n\
    soft_logo_name   TEXT,\r\n\
    onekey_qparam    TEXT,\r\n\
    file_md5         TEXT,\r\n\
    file_size        INTEGER,\r\n\
    current_progress INTEGER,\r\n\
    current_speed    INTEGER,\r\n\
    remain_ime       INTEGER,\r\n\
    task_state       INTEGER,\r\n\
    reserved1        INTEGER,\r\n\
    reserved2        INTEGER,\r\n\
    reserved3        TEXT,\r\n\
    reserved4        TEXT,\r\n\
    reserved5        TEXT \r\n\
);");
}

std::string CSoftDownInfoTable::GetInsertSql() const 
{
	return TU::CTableUtil::StringFormat( "insert into SoftDownInfoTable( soft_id, soft_name, versions, soft_down_path, soft_down_url, soft_logo_name, onekey_qparam, file_md5, file_size, current_progress, current_speed, remain_ime, task_state, reserved1, reserved2, reserved3, reserved4, reserved5) values( %d, \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', %d, %d, %d, %d, %d, %d, %d, \'%s\', \'%s\', \'%s\');", nSoftId, TU::CTableUtil::StringTransf( sSoftName ).c_str(), TU::CTableUtil::StringTransf( sVersions ).c_str(), TU::CTableUtil::StringTransf( sSoftDownPath ).c_str(), TU::CTableUtil::StringTransf( sSoftDownUrl ).c_str(), TU::CTableUtil::StringTransf( sSoftLogoName ).c_str(), TU::CTableUtil::StringTransf( sOnekeyQparam ).c_str(), TU::CTableUtil::StringTransf( sFileMd5 ).c_str(), nFileSize, nCurrentProgress, nCurrentSpeed, nRemainIme, nTaskState, nReserved1, nReserved2, TU::CTableUtil::StringTransf( sReserved3 ).c_str(), TU::CTableUtil::StringTransf( sReserved4 ).c_str(), TU::CTableUtil::StringTransf( sReserved5 ).c_str());
}

std::string CSoftDownInfoTable::GetRemoveSql() const 
{
	return TU::CTableUtil::StringFormat( "delete from SoftDownInfoTable where soft_id=%d ;", nSoftId);
}

std::string CSoftDownInfoTable::GetUpdateSql() const 
{
	return TU::CTableUtil::StringFormat( "update SoftDownInfoTable set  soft_id=%d, soft_name=\'%s\', versions=\'%s\', soft_down_path=\'%s\', soft_down_url=\'%s\', soft_logo_name=\'%s\', onekey_qparam=\'%s\', file_md5=\'%s\', file_size=%d, current_progress=%d, current_speed=%d, remain_ime=%d, task_state=%d, reserved1=%d, reserved2=%d, reserved3=\'%s\', reserved4=\'%s\', reserved5=\'%s\' where soft_id=%d;", nSoftId, TU::CTableUtil::StringTransf( sSoftName ).c_str(), TU::CTableUtil::StringTransf( sVersions ).c_str(), TU::CTableUtil::StringTransf( sSoftDownPath ).c_str(), TU::CTableUtil::StringTransf( sSoftDownUrl ).c_str(), TU::CTableUtil::StringTransf( sSoftLogoName ).c_str(), TU::CTableUtil::StringTransf( sOnekeyQparam ).c_str(), TU::CTableUtil::StringTransf( sFileMd5 ).c_str(), nFileSize, nCurrentProgress, nCurrentSpeed, nRemainIme, nTaskState, nReserved1, nReserved2, TU::CTableUtil::StringTransf( sReserved3 ).c_str(), TU::CTableUtil::StringTransf( sReserved4 ).c_str(), TU::CTableUtil::StringTransf( sReserved5 ).c_str(), nSoftId);
}

std::string CSoftDownInfoTable::GetLookupSql() const 
{
	std::string sSQl = "select * from SoftDownInfoTable where 1=1";
	if ( 0 != nSoftId )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and soft_id=%d", nSoftId ) );
	}

	if ( "" != sSoftName )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and soft_name=\'%s\'", TU::CTableUtil::StringTransf( sSoftName ).c_str() ) );
	}

	if ( "" != sVersions )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and versions=\'%s\'", TU::CTableUtil::StringTransf( sVersions ).c_str() ) );
	}

	if ( "" != sSoftDownPath )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and soft_down_path=\'%s\'", TU::CTableUtil::StringTransf( sSoftDownPath ).c_str() ) );
	}

	if ( "" != sSoftDownUrl )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and soft_down_url=\'%s\'", TU::CTableUtil::StringTransf( sSoftDownUrl ).c_str() ) );
	}

	if ( "" != sSoftLogoName )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and soft_logo_name=\'%s\'", TU::CTableUtil::StringTransf( sSoftLogoName ).c_str() ) );
	}

	if ( "" != sOnekeyQparam )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and onekey_qparam=\'%s\'", TU::CTableUtil::StringTransf( sOnekeyQparam ).c_str() ) );
	}

	if ( "" != sFileMd5 )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and file_md5=\'%s\'", TU::CTableUtil::StringTransf( sFileMd5 ).c_str() ) );
	}

	if ( 0 != nFileSize )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and file_size=%d", nFileSize ) );
	}

	if ( 0 != nCurrentProgress )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and current_progress=%d", nCurrentProgress ) );
	}

	if ( 0 != nCurrentSpeed )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and current_speed=%d", nCurrentSpeed ) );
	}

	if ( 0 != nRemainIme )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and remain_ime=%d", nRemainIme ) );
	}

	if ( 0 != nTaskState )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and task_state=%d", nTaskState ) );
	}

	if ( 0 != nReserved1 )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and reserved1=%d", nReserved1 ) );
	}

	if ( 0 != nReserved2 )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and reserved2=%d", nReserved2 ) );
	}

	if ( "" != sReserved3 )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and reserved3=\'%s\'", TU::CTableUtil::StringTransf( sReserved3 ).c_str() ) );
	}

	if ( "" != sReserved4 )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and reserved4=\'%s\'", TU::CTableUtil::StringTransf( sReserved4 ).c_str() ) );
	}

	if ( "" != sReserved5 )
	{
		sSQl.append( TU::CTableUtil::StringFormat( " and reserved5=\'%s\'", TU::CTableUtil::StringTransf( sReserved5 ).c_str() ) );
	}

	sSQl.append(";");
	return sSQl;
}

void CSoftDownInfoTable::BuildTableInfo( std::map<std::string,std::string> TableInfoMap )
{
	nSoftId=atoi(TableInfoMap["soft_id"].c_str());
	sSoftName=TableInfoMap["soft_name"];
	sVersions=TableInfoMap["versions"];
	sSoftDownPath=TableInfoMap["soft_down_path"];
	sSoftDownUrl=TableInfoMap["soft_down_url"];
	sSoftLogoName=TableInfoMap["soft_logo_name"];
	sOnekeyQparam=TableInfoMap["onekey_qparam"];
	sFileMd5=TableInfoMap["file_md5"];
	nFileSize=atoi(TableInfoMap["file_size"].c_str());
	nCurrentProgress=atoi(TableInfoMap["current_progress"].c_str());
	nCurrentSpeed=atoi(TableInfoMap["current_speed"].c_str());
	nRemainIme=atoi(TableInfoMap["remain_ime"].c_str());
	nTaskState=atoi(TableInfoMap["task_state"].c_str());
	nReserved1=atoi(TableInfoMap["reserved1"].c_str());
	nReserved2=atoi(TableInfoMap["reserved2"].c_str());
	sReserved3=TableInfoMap["reserved3"];
	sReserved4=TableInfoMap["reserved4"];
	sReserved5=TableInfoMap["reserved5"];

}

