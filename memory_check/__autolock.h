#ifndef __LOCK_HH__
#define __LOCK_HH__

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>  
#include <pthread.h>  
#endif

namespace ns_lock
{

    /*********************************************************************************************/
#ifdef WIN32
    class _Lock  //临界区是可以递归的可以重入的锁
    {
    public:
        _Lock() { InitializeCriticalSection(&__cs); }
        ~_Lock() { DeleteCriticalSection(&__cs); }
    
        void lock() { EnterCriticalSection(&__cs); }
        void unlock() { LeaveCriticalSection(&__cs); }

    private:
        CRITICAL_SECTION __cs;
    };

#else //linux

    class _Lock  //Linux下的pthread_mutex_t 默认是不可递归的, 初始化时设置PTHREAD_MUTEX_RECURSIVE属性后，可以重入
    {
    public:
        _Lock() { 
            //__mutex = PTHREAD_MUTEX_INITIALIZER; 
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

            pthread_mutex_init(&__mutex, &attr);
        }

        ~_Lock() { pthread_mutex_destroy(&__mutex); }

        void lock() { pthread_mutex_lock(&__mutex); }
        void unlock() { pthread_mutex_unlock(&__mutex); }

    private:
        pthread_mutex_t __mutex;
    };
#endif

    /*********************************************************************************************/
    //自动锁
    class _Auto_Lock
    {
    public:
        _Auto_Lock(_Lock * lk) : __lock(lk) { __lock->lock(); }
        ~_Auto_Lock() { __lock->unlock(); }
    private:
        _Lock * __lock;
    };

    /*********************************************************************************************/
#if 0
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/syscall.h>
#endif
    inline unsigned int get_thread_id()
    {
#ifdef _WIN32
        return (unsigned int)::GetCurrentThreadId();
#else
        return (unsigned int)syscall(SYS_gettid);
#endif
    }

    //可重入锁, 同一个线程可以重入，不同线程不能同时获取到锁
    class _ReenteryLock : public _Lock
    {
    public:
        _ReenteryLock() : _Lock(), _count(0), _master(0) {}
        ~_ReenteryLock() {}

        void _ReenteryLock::lock()
        {
            unsigned int curthreadid = get_thread_id();
            assert(curthreadid != 0);
            if (curthreadid == _master)
            {
                ++_count;
            }
            else
            {
                //不同的线程继续等锁
                //_originallock.lock();
                _Lock::lock();
                _count = 1; //++_count;  // _count = 1;
                _master = curthreadid;
            }
}

        void _ReenteryLock::unlock()
        {
            unsigned int curthreadid = get_thread_id();
            assert(curthreadid != 0);
            if (curthreadid == _master)
            {
                --_count;
                if (_count == 0)
                {
                    _master = 0;
                    //_originallock.unlock();
                    _Lock::unlock();
                }
            }
        }

    private:
        //_Lock _originallock;   //原始锁（非可重入）
        unsigned int _count;   //锁被同一线程获取的次数
        unsigned int _master;  //锁被占用的线程 

    };
#endif  //#if 0

    /*********************************************************************************************/

} //namespace ns_lock

#endif //__LOCK_HH__
