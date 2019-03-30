#ifndef __MEMORY_CHECK_H_H__
#define __MEMORY_CHECK_H_H__

#ifdef MEMORY_CHECK
void * operator new(size_t size, const char * file, const char * func, unsigned int line);
void * operator new[](size_t size, const char * file, const char * func, unsigned int line);
//void operator delete(void * ptr, const char * file, const char * func, unsigned int line);
//void operator delete[](void * ptr, const char * file, const char * func, unsigned int line);
void operator delete(void * ptr);
void operator delete[](void * ptr);

#ifndef __DEBUG_MACRO_NEW_DELETE_201903292259__

#ifndef new
#define new new(__FILE__, __FUNCTION__, __LINE__)
#endif //new

#ifndef delete
//#define delete delete(__FILE__, __FUNCTION__, __LINE__)
#endif //delete

void PrintMemCheckResult();
void FreeMemCheck();

#endif //__DEBUG_MACRO_NEW_DELETE_201903292259__
#endif //MEMORY_CHECK

#endif //__MEMORY_CHECK_H_H__
