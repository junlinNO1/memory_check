#ifndef __INTERNAL_MEMORY_CHECK_H_H__
#define __INTERNAL_MEMORY_CHECK_H_H__

#ifndef MEMORY_CHECK
#define MEMORY_CHECK
#endif //MEMORY_CHECK

#ifndef __DEBUG_MACRO_NEW_DELETE_201903292259__
#define __DEBUG_MACRO_NEW_DELETE_201903292259__
#endif //__DEBUG_MACRO_NEW_DELETE_201903292259__

#include "memory_check.h"
#include "__stack.h"
#include "__autolock.h"

enum MEM_OP_TYPE
{
    OP_NEW = 0,
    OP_NEW_ARRAY,
    OP_DELETE,
    OP_DELETE_ARRAY,
    OP_NONE
};

enum MEM_STATE
{
    MEM_STATE_ALLOCATED = 0,      //已分配
    MEM_STATE_REPEAT_ALLOCATED,   //同一地址重复分配
    MEM_STATE_RELEASED,           //已经正确释放
    MEM_STATE_REPEAT_RELEASED,    //重复释放
    MEM_STATE_UNKNOW_ADDR,        //未知地址释放
    MEM_STATE_DELETE_NOT_MATCH    //delete/delete[]与new[]/new不匹配
};

typedef struct _meminfo
{
    void *       _ptr;
    unsigned int _size;
    const char * _file;
    const char * _func;
    unsigned int _line;
    MEM_OP_TYPE  _type;
    unsigned int _state;

    _meminfo * _next;

    //默认构造
    _meminfo() :_ptr(NULL)
        , _size(0)
        , _file(NULL)
        , _func(NULL)
        , _line(0)
        , _type(OP_NONE)
        , _state(0)
        , _next(NULL)
    {}

    //带参构造
    _meminfo(void * ptr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
        :_ptr(ptr)
        , _size(size)
        , _file(file)
        , _func(func)
        , _line(line)
        , _type(type)
        , _state(0)
        , _next(NULL)
    {}

} MemInfoT;


class MemoryCheck
{
public:
    MemoryCheck();
    ~MemoryCheck();

    void InsertMemInfo(void * ptr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type);
    void DeleteMemInfo(void * ptr, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type);
    void DeleteMemInfo(void * ptr, MEM_OP_TYPE type);
 
    void PrintMemCheckResult();
    void FreeMemInfo();
    void PrintBacktrace();

    void SetDeleteInfo(const char * file, const char * func, unsigned int line);
    
private:
    void FreeMemInfo(MemInfoT *& meminfo);
    void FindMemInfo(void * ptr, MemInfoT *& meminfo);
    void InsertErrorInfo(MemInfoT meminfo);
    void GetBacktrace(char * buff, unsigned int maxlen);
    
    void PrintMemLeak();
    void PrintRepeatRelease();
    void PrintReleaseSucceed();
    void PrintReleaseTypeNotMatch();
    void PrintReleaseUnknowAddr();

private:
    MemInfoT * newlist;
    MemInfoT * errorlist;
    SimpleLinkStack _stack;

    ns_lock::_Lock _newlock;
    ns_lock::_Lock _errlock;
    ns_lock::_Lock _deletelock;
};

#endif //__INTERNAL_MEMORY_CHECK_H_H__