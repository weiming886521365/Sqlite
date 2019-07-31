#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#endif

#include <string>

namespace SYNC {

class CMutexLock 
{
   public :
   
      class Owner
      {
         public:

            explicit Owner(CMutexLock &crit);

            ~Owner();
      
         private :

            CMutexLock &m_crit;

            // No copies do not implement
            Owner(const Owner &rhs);
            Owner &operator=(const Owner &rhs);
      };

      CMutexLock( std::string sMutexName = "" );
      
      ~CMutexLock();

#if(_WIN32_WINNT >= 0x0400)
      bool TryEnter();
#endif //_WIN32_WINNT >= 0x0400
      void Enter();

      void Leave();

   private :
	   std::string m_sMutexName;

#ifndef WIN32
	   pthread_mutex_t m_Mutex;
#else
	   CRITICAL_SECTION m_Mutex;
	   HANDLE m_pMutex;
#endif //WIN32

      // No copies do not implement
      CMutexLock(const CMutexLock &rhs);
      CMutexLock &operator=(const CMutexLock &rhs);
};

}
#endif //__MUTEXLOCK_H__
