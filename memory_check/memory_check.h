#ifndef __MEMORY_CHECK_H_H__
#define __MEMORY_CHECK_H_H__

#ifdef MEMORY_CHECK   //如果需要进行内存分配释放检测时，虚在引入此头文件前先定义此宏

void * operator new(size_t size, const char * file, const char * func, unsigned int line);
void * operator new[](size_t size, const char * file, const char * func, unsigned int line);
void operator delete(void * ptr, const char * file, const char * func, unsigned int line);
void operator delete[](void * ptr, const char * file, const char * func, unsigned int line);
void operator delete(void * ptr)  throw ();
void operator delete[](void * ptr)  throw ();

#ifndef __DEBUG_MACRO_NEW_DELETE_201903292259__

#ifndef new
#define new new(__FILE__, __FUNCTION__, __LINE__)
#endif //new

#ifndef delete
//#define delete delete(__FILE__, __FUNCTION__, __LINE__)
#endif //delete

//打印内存分配释放检测结果
void PrintMemCheckResult(); 
//释放内存检测过程中分配的资源
void FreeMemCheck();

//主要用在下面的宏中，用来传递delete/delete[]的文件名、函数名、行号信息
void SetDeleteInfo(const char * file, const char * func, unsigned int line);

#ifndef delete
#define delete SetDeleteInfo(__FILE__, __FUNCTION__, __LINE__);delete
#endif //delete

#endif //__DEBUG_MACRO_NEW_DELETE_201903292259__

#endif //MEMORY_CHECK

#endif //__MEMORY_CHECK_H_H__
