#ifndef __INTERNAL_MEMORY_CHECK_H_H__
#define __INTERNAL_MEMORY_CHECK_H_H__

//#ifndef __MEMORY_CHECK_LIJUN__
//#define __MEMORY_CHECK_LIJUN__
//#endif //__MEMORY_CHECK_LIJUN__

//#ifndef __DEBUG_MACRO_NEW_DELETE_201903292259__
//#define __DEBUG_MACRO_NEW_DELETE_201903292259__
//#endif //__DEBUG_MACRO_NEW_DELETE_201903292259__

//#include "memory_check.h"
#include "__stack.h"
#include "__autolock.h"
#include "__memory_info.h"
#include "__hash.h"

class MemoryCheck
{
public:
    MemoryCheck();
    ~MemoryCheck();

    int InsertMemInfo(void * ptr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type);
    int DeleteMemInfo(void * ptr, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type);
    int DeleteMemInfo(void * ptr, MEM_OP_TYPE type);
    int InsertMemInfo(void * ptr, size_t size, MEM_OP_TYPE type);
 
    void PrintMemCheckResult();
    void FreeMemInfo();
    void PrintBacktrace();

    void SetDeleteInfo(const char * file, const char * func, unsigned int line);
    void SetNewInfo(const char * file, const char * func, unsigned int line);
    void GetDeleteLock();
    void ReleaseDeleteLock();

private:
    void FreeMemInfo(MemInfoT *& meminfo);
    void FindMemInfo(void * ptr, MemInfoT *& meminfo);
    void FindAndDeleteMemInfo(void * ptr, MemInfoT *& meminfo);
    void InsertErrorInfo(MemInfoT meminfo);
    void GetBacktrace(char * buff, unsigned int maxlen);
    
    void PrintMemLeak();
    void PrintRepeatedRelease();
    void PrintReleaseSucceed();
    void PrintReleaseTypeMismatch();
    void PrintReleaseUnknowAddr();

private:
    MemInfoT * _newlist;
    MemInfoT * _errorlist;
    MemInfoT * _not_trace_newlist;
    SimpleLinkStack _stack;
    //SimpleLinkStack _stack_for_malloc;

    ns_lock::_Lock _newlock;
    ns_lock::_Lock _errlock;
    ns_lock::_Lock _deletelock;
    ns_lock::_Lock _not_trace_newlock;

    _HashTable _new_hash_table;
    _HashTable _not_trace_new_hash_table;
};

#endif //__INTERNAL_MEMORY_CHECK_H_H__