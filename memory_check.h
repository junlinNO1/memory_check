#ifndef __MEMORY_CHECK_H_H__
#define __MEMORY_CHECK_H_H__

#ifdef __MEMORY_CHECK_LIJUN__   //�����Ҫ�����ڴ�����ͷż��ʱ�����������ͷ�ļ�ǰ�ȶ���˺�
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
* ��������:  operator delete
* ����:	 lijun
* ����  2019.03.29
* ����:
* ����:  ���� operator delete   ע�⣺������static�ģ�����ȫ�ַ�Χ�ڵ�delete�����õ��˺���
* ����ֵ:  void
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
* ��������:  operator delete[]
* ����:	 lijun
* ����  2019.03.29
* ����:
* ����:  ���� operator delete[]   ע�⣺������static�ģ�����ȫ�ַ�Χ�ڵ�delete�����õ��˺���
* ����ֵ:  void
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
* ��������: PrintMemCheckResult
* ����:	   lijun
* ���ڣ�   2019.03.29
* ����:
* ����:    ��ӡ�ڴ�����ͷż����
* ����ֵ:  void
******************************************************************************/
extern void PrintMemCheckResult(); 

/******************************************************************************
* ��������: FreeMemCheck
* ����:	   lijun
* ���ڣ�   2019.03.29
* ����:
* ����:    �ͷ��ڴ�������з������Դ
* ����ֵ:  void
******************************************************************************/
extern void FreeMemCheck();


//��Ҫ��������ĺ��У���������delete/delete[]���ļ��������������к���Ϣ
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
