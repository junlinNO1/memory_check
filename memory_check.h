#ifndef __MEMORY_CHECK_H_H__
#define __MEMORY_CHECK_H_H__

#ifdef __MEMORY_CHECK_LIJUN__   //如果需要进行内存分配释放检测时，虚在引入此头文件前先定义此宏
#include <iostream>

extern void * operator new(size_t size, const char * file, const char * func, unsigned int line);
extern void * operator new[](size_t size, const char * file, const char * func, unsigned int line);
extern void * operator new(size_t size);
extern void * operator new[](size_t size);

extern void operator delete(void * ptr, const char * file, const char * func, unsigned int line);
extern void operator delete[](void * ptr, const char * file, const char * func, unsigned int line);
extern void operator delete(void * ptr)  throw ();
extern void operator delete[](void * ptr)  throw ();

extern int DeleteMemInfo(void * ptr, int type);
/******************************************************************************
* 函数名称:  operator delete
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载 operator delete   注意：这里是static的，以免全局范围内的delete都调用到此函数
* 返回值:  void
******************************************************************************/
static void operator delete(void * ptr) throw()
{
    if (ptr == NULL)
        return;

    if (0 == DeleteMemInfo(ptr, 2))
    {
        free(ptr);
    }
}

/******************************************************************************
* 函数名称:  operator delete[]
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载 operator delete[]   注意：这里是static的，以免全局范围内的delete都调用到此函数
* 返回值:  void
******************************************************************************/
static void operator delete[](void * ptr) throw()
{
    if (ptr == NULL)
        return;

    if (0 == DeleteMemInfo(ptr, 3))
    {
        free(ptr);
    }
}

#ifndef __DEBUG_MACRO_NEW_DELETE_201903292259__

#ifndef new
#define new new(__FILE__, __FUNCTION__, __LINE__)
#endif //new

#ifndef delete
//#define delete delete(__FILE__, __FUNCTION__, __LINE__)
#endif //delete

/******************************************************************************
* 函数名称: PrintMemCheckResult
* 作者:	   lijun
* 日期：   2019.03.29
* 参数:
* 功能:    打印内存分配释放检测结果
* 返回值:  void
******************************************************************************/
extern void PrintMemCheckResult(); 

/******************************************************************************
* 函数名称: FreeMemCheck
* 作者:	   lijun
* 日期：   2019.03.29
* 参数:
* 功能:    释放内存检测过程中分配的资源
* 返回值:  void
******************************************************************************/
extern void FreeMemCheck();


//主要用在下面的宏中，用来传递delete/delete[]的文件名、函数名、行号信息
extern void SetDeleteInfo(const char * file, const char * func, unsigned int line);

#ifndef delete
#define delete SetDeleteInfo(__FILE__, __FUNCTION__, __LINE__);delete
#endif //delete

#endif //__DEBUG_MACRO_NEW_DELETE_201903292259__

#endif //MEMORY_CHECK


#ifdef __MEMORY_CHECK_LIJUN_C__

extern void * malloc_memory_check(size_t size, const char * file, const char * func, unsigned int line);
extern void free_memory_check(void * ptr, const char * file, const char * func, unsigned int line);

#ifndef malloc
#define malloc(szie) malloc_memory_check(szie, __FILE__, __FUNCTION__, __LINE__)
#endif

#ifndef free
#define free(ptr) free_memory_check(ptr, __FILE__, __FUNCTION__, __LINE__)

#endif

#endif //__MEMORY_CHECK_LIJUN_C__

#endif //__MEMORY_CHECK_H_H__
