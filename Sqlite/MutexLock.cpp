
#include "stdafx.h"
#include "MutexLock.h"

///////////////////////////////////////////////////////////////////////////////
// CMutexLock
///////////////////////////////////////////////////////////////////////////////

namespace SYNC {

CMutexLock::CMutexLock( std::string sMutexName )
{
	m_sMutexName = sMutexName;
#ifndef WIN32
	::pthread_mutex_init(&this->m_Mutex, NULL);
#else
	if ( 0 == m_sMutexName.size() )
	{
		::InitializeCriticalSection(&this->m_Mutex);
	}
	else
	{
		m_pMutex = CreateEventA( NULL, FALSE, TRUE, m_sMutexName.c_str());
	}
#endif //WIN32
}

CMutexLock::~CMutexLock()
{
#ifndef WIN32
	::pthread_mutex_destroy(&this->m_Mutex);
#else
	if ( 0 == m_sMutexName.size() )
	{
		::DeleteCriticalSection(&this->m_Mutex);
	}
	else
	{
		CloseHandle( m_pMutex );
	}
#endif //WIN32
}

#if(_WIN32_WINNT >= 0x0400)
bool CMutexLock::TryEnter()
{
	bool bRet = false;

#ifndef WIN32
	if (::pthread_mutex_trylock(&this->m_Mutex) == 0) bRet = true;
#else
	if ( 0 == m_sMutexName.size() )
	{
		if (::TryEnterCriticalSection(&this->m_Mutex)) bRet = true;
	}
	else
	{
		if ( WAIT_OBJECT_0 == WaitForSingleObject( m_pMutex,0 ) )
		{
			bRet = true;
			ResetEvent(m_pMutex);
		}
	}
#endif //WIN32

	return bRet;
}
#endif

void CMutexLock::Enter()
{
#ifndef WIN32
	::pthread_mutex_lock(&this->m_Mutex);
#else
	if ( 0 == m_sMutexName.size() )
	{
		::EnterCriticalSection(&this->m_Mutex);
	}
	else
	{
		WaitForSingleObject( m_pMutex, INFINITE );
	}
#endif //WIN32
}

void CMutexLock::Leave()
{
#ifndef WIN32
	::pthread_mutex_unlock(&this->m_Mutex);
#else
	if ( 0 == m_sMutexName.size() )
	{
		::LeaveCriticalSection(&this->m_Mutex);
	}
	else
	{
		SetEvent(m_pMutex);
	}
#endif //WIN32
}

///////////////////////////////////////////////////////////////////////////////
// CMutexLock::Owner
///////////////////////////////////////////////////////////////////////////////

CMutexLock::Owner::Owner(
	CMutexLock &crit)
	: m_crit(crit)
{
	m_crit.Enter();
}

CMutexLock::Owner::~Owner()
{
	m_crit.Leave();
} 

};
