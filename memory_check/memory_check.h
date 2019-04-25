#ifndef __MEMORY_CHECK_H_H__
#define __MEMORY_CHECK_H_H__

#ifdef MEMORY_CHECK   //�����Ҫ�����ڴ�����ͷż��ʱ�����������ͷ�ļ�ǰ�ȶ���˺�

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

//��ӡ�ڴ�����ͷż����
void PrintMemCheckResult(); 
//�ͷ��ڴ�������з������Դ
void FreeMemCheck();

//��Ҫ��������ĺ��У���������delete/delete[]���ļ��������������к���Ϣ
void SetDeleteInfo(const char * file, const char * func, unsigned int line);

#ifndef delete
#define delete SetDeleteInfo(__FILE__, __FUNCTION__, __LINE__);delete
#endif //delete

#endif //__DEBUG_MACRO_NEW_DELETE_201903292259__

#endif //MEMORY_CHECK

#endif //__MEMORY_CHECK_H_H__
