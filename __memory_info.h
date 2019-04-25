#ifndef __MEMEORY_INFO_STRUCT_H_H__
#define __MEMEORY_INFO_STRUCT_H_H__
#include <stdio.h>
#include <stdlib.h>

enum MEM_OP_TYPE
{
    OP_NEW = 0,
    OP_NEW_ARRAY,
    OP_DELETE,
    OP_DELETE_ARRAY,
    OP_MALLOC,
    OP_FREE,
    OP_NONE
};

enum MEM_STATE
{
    MEM_STATE_ALLOCATED = 0,      //已分配
    MEM_STATE_REPEAT_ALLOCATED,   //同一地址重复分配
    MEM_STATE_RELEASED,           //已经正确释放
    MEM_STATE_REPEATED_RELEASED,  //重复释放
    MEM_STATE_UNKNOW_ADDR,        //未知地址释放
    MEM_STATE_DELETE_MISMATCH     //delete/delete[]与new[]/new不匹配
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
    MEM_OP_TYPE  _expect_type;

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
    _meminfo(void * ptr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type, 
        MEM_OP_TYPE expect_type = OP_NONE, unsigned int state = 0, _meminfo * next = NULL)
        :_ptr(ptr)
        , _size(size)
        , _file(file)
        , _func(func)
        , _line(line)
        , _type(type)
        , _expect_type(expect_type)
        , _state(state)
        , _next(next)
    {}

} MemInfoT;

#endif //__MEMEORY_INFO_STRUCT_H_H__
