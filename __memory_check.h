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

    int InsertMemInfo(unsigned long addr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type);
    int InsertMemInfo(unsigned long addr, size_t size, MEM_OP_TYPE type);
    
    int DeleteMemInfo(unsigned long addr, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type);
    int DeleteMemInfo(unsigned long addr, MEM_OP_TYPE type);
    
    void PrintMemCheckResult();
    void FreeMemInfo();

private:
    void FreeMemInfo(MemInfoT *& meminfo);
    void InsertErrorInfo(MemInfoT meminfo);
    void PrintBacktrace(char * format, ...);
    void GetBacktrace(char * buff, unsigned int maxlen);
    
    void PrintMemLeak();
    void PrintRepeatedRelease();
    void PrintReleaseSucceed();
    void PrintReleaseTypeMismatch();
    void PrintReleaseUnknowAddr();

    void WriteToFile(char * format, ...);

private:
    MemInfoT * _errorlist;
    SimpleLinkStack _stack;

    ns_lock::_Lock _errlock;

    _HashTable _new_hash_table;
    _HashTable _not_trace_new_hash_table;

    FILE * _correct_released_file;
    FILE * _incorrect_released_bt;
    FILE * _incorrect_released_file;
};

#endif //__INTERNAL_MEMORY_CHECK_H_H__