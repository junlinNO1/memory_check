﻿#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "__memory_check.h"

#ifdef _WIN32
#else

#endif

#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
//define something for Windows (64-bit only)
#else
#include "__windows_print_backtrace.h"
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#   error "Unknown Apple platform"
#endif
#elif __ANDROID__
// android
#elif __linux__
#include <unistd.h>
#include <execinfo.h>
#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#   error "Unknown compiler"
#endif

using namespace std;

static const char * OP_NAME[] = { "new", "new[]", "delete", "delete[]", "malloc", "free", "unknow" };

//全局内存检测对象
static MemoryCheck MemCheck;

/******************************************************************************
* 函数名称: operator new
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  重载全局的 operator new, 用于记录分配内存的位置等信息
* 返回 值:  void
******************************************************************************/
void * operator new(size_t size, const char * file, const char * func, unsigned int line)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(ptr, size, file, func, line, OP_NEW);
    return ptr;
}

/******************************************************************************
* 函数名称: operator new[]
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  重载全局的 operator new[], 用于记录分配内存的位置等信息
* 返回 值:  void
******************************************************************************/
void * operator new[](size_t size, const char * file, const char * func, unsigned int line)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(ptr, size, file, func, line, OP_NEW_ARRAY);
    return ptr;
}
#if 0
/******************************************************************************
* 函数名称: operator new
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  重载全局的 operator new, 用于记录分配内存的位置等信息
* 返回 值:  void
******************************************************************************/
void * operator new(size_t size)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(ptr, size, OP_NEW);
    return ptr;
}

/******************************************************************************
* 函数名称: operator new[]
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  重载全局的 operator new[], 用于记录分配内存的位置等信息
* 返回 值:  void
******************************************************************************/
void * operator new[](size_t size)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(ptr, size, OP_NEW_ARRAY);
    return ptr;
}
#endif
#if 1
/******************************************************************************
* 函数名称:  operator delete
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载全局的 operator delete
* 返回值:  void
******************************************************************************/
void operator delete(void * ptr, const char * file, const char * func, unsigned int line)
{
    printf("[MemoryCheck][Malloc_Error]operator delete(void * ptr, const char * file, const char * func, unsigned int line)!.\n");
    if (ptr == NULL) 
        return;

    if (0 == MemCheck.DeleteMemInfo(ptr, file, func, line, OP_DELETE))
    {
        free(ptr);
    }
}

/******************************************************************************
* 函数名称:  operator delete
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载全局的 operator delete
* 返回值:  void
******************************************************************************/
void operator delete[](void * ptr, const char * file, const char * func, unsigned int line)
{
    printf("[MemoryCheck][Malloc_Error]operator delete[](void * ptr, const char * file, const char * func, unsigned int line)!.\n");
    if (ptr == NULL)
        return;

    if (0 == MemCheck.DeleteMemInfo(ptr, file, func, line, OP_DELETE_ARRAY))
    {
        free(ptr);
    }
}
#endif

/******************************************************************************
* 函数名称:  operator delete
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载全局的 operator delete
* 返回值:  void
******************************************************************************/
#if 0
void operator delete(void * ptr) throw()
{
    if (ptr == NULL)
        return;
    //MemCheck.GetDeleteLock();
    free(ptr);

#ifdef __MEMORY_CHECK_LIJUN__
    MemCheck.DeleteMemInfo(ptr, OP_DELETE);
#endif
}
#endif

/******************************************************************************
* 函数名称:  operator delete[]
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载全局的 operator delete[]
* 返回值:  void
******************************************************************************/
#if 0
void operator delete[](void * ptr) throw()
{
    if (ptr == NULL)
        return;
    //MemCheck.GetDeleteLock();
    free(ptr);

#ifdef __MEMORY_CHECK_LIJUN__
    MemCheck.DeleteMemInfo(ptr, OP_DELETE_ARRAY);
#endif
}
#endif

/******************************************************************************
* 函数名称: malloc_memory_check
* 作   者:  lijun
* 日   期:  2019.04.10
* 参   数:
* 功   能:  模拟malloc分配内存
* 返回 值:  void
******************************************************************************/
void * malloc_memory_check(size_t size, const char * file, const char * func, unsigned int line)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    //MemCheck.InsertMemInfo(ptr, size, OP_MALLOC);
    MemCheck.InsertMemInfo(ptr, size, file, func, line, OP_MALLOC);
    return ptr;
}

/******************************************************************************
* 函数名称: free_memory_check
* 作   者:  lijun
* 日   期:  2019.04.10
* 参   数:
* 功   能:  模拟free释放内存
* 返回 值:  void
******************************************************************************/
void free_memory_check(void * ptr, const char * file, const char * func, unsigned int line)
{
    if (ptr == NULL)
        return;

    //if (0 == MemCheck.DeleteMemInfo(ptr, OP_FREE))
    if (0 == MemCheck.DeleteMemInfo(ptr, file, func, line, OP_FREE))
    {
        free(ptr);
    }
}

/******************************************************************************
* 函数名称:  PrintMemCheckResult
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  全局对外接口，打印内存检测结果
* 返回值:  void
******************************************************************************/
void PrintMemCheckResult()
{
    MemCheck.PrintMemCheckResult();
}

/******************************************************************************
* 函数名称: FreeMemCheck
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  全局对外接口，打印内存检测结果
* 返回 值:  void
******************************************************************************/
void FreeMemCheck()
{
    MemCheck.FreeMemInfo();
}

/******************************************************************************
* 函数名称: SetDeleteInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  设置释放内存时的位置信息
* 返回 值:  void
******************************************************************************/
void SetDeleteInfo(const char * file, const char * func, unsigned int line)
{
    MemCheck.SetDeleteInfo(file, func, line);
}

/******************************************************************************
* 函数名称: SetDeleteInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  释放内存，  对外接口
* 返回 值:  void
******************************************************************************/
int DeleteMemInfo(void * ptr, int type)
{
    return MemCheck.DeleteMemInfo(ptr, (MEM_OP_TYPE)type);
}

/******************************************************************************
* 函数名称: SetNewInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  设置分配内存时的位置信息
* 返回 值:  void
******************************************************************************/
void SetNewInfo(const char * file, const char * func, unsigned int line)
{
    MemCheck.SetNewInfo(file, func, line);
}

/******************************************************************************
* 函数名称: MemoryCheck::MemoryCheck
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  构造函数
* 返回 值:  void
******************************************************************************/

MemoryCheck::MemoryCheck() : _newlist(NULL), _errorlist(NULL), _not_trace_newlist(NULL)
{
    //InitTrack();
}

/******************************************************************************
* 函数名称: MemoryCheck::~MemoryCheck
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  析构函数，释放内存分配信息链表
* 返回 值:  void
******************************************************************************/

MemoryCheck::~MemoryCheck()
{
    FreeMemInfo();
    //UninitTrack();
}

/******************************************************************************
* 函数名称: MemoryCheck::InsertMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  将内存分配信息插入分配链表
* 返回 值:  int
******************************************************************************/
int MemoryCheck::InsertMemInfo(void * ptr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
{
    assert(ptr != NULL);
    MemInfoT * meminfo = NULL;
    //ns_lock::_Auto_Lock autolock(&_newlock);
    _new_hash_table.FindElement(ptr, meminfo);

    if (meminfo == NULL)
    {
        MemInfoT tmp = MemInfoT(ptr, size, file, func, line, type, OP_NONE, MEM_STATE_ALLOCATED);
        _new_hash_table.InsertElement(&tmp);
    }
    else
    {
        if (meminfo->_state != MEM_STATE_ALLOCATED && meminfo->_state != MEM_STATE_REPEAT_ALLOCATED)
        {
            //源节点已经释放过(无论当时释放是否出错)，则更新节点信息，重复利用节点
            *meminfo = MemInfoT(ptr, size, file, func, line, type, OP_NONE, MEM_STATE_ALLOCATED, meminfo->_next);
        }
        else  //理论上不可能出现前一次未释放，而下一次分配在同一个堆地址上
        {
            printf("[MemoryCheck][Malloc_Error]InsertMemInfo, malloc mem repeat!.\n");
            MemInfoT temp = MemInfoT(ptr, size, file, func, line, type);
            temp._state = MEM_STATE_REPEAT_ALLOCATED; //置节点状态为重复分配
            InsertErrorInfo(temp);
        }
    }
    return 0;
}

/******************************************************************************
* 函数名称: MemoryCheck::FindMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:  void * ptr 内存分配地址 
           MemInfoT *& meminfo  输出查找到的内存分配信息
* 功   能:  在分配列表中查找是否存在同地址的分配信息
* 返回 值:  void
******************************************************************************/
void MemoryCheck::FindMemInfo(void * ptr, MemInfoT *& meminfo)
{
    assert(ptr != NULL);
    meminfo = NULL;
    //ns_lock::_Auto_Lock autolock(&_newlock);
    MemInfoT * curr = _newlist;
    while (curr != NULL && curr->_ptr != ptr)
    {
        curr = curr->_next;
    }

    //找到同地址的分配信息
    if (curr != NULL)
    {
        meminfo = curr;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::FindAndDeleteMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:  void * ptr 内存分配地址
MemInfoT *& meminfo  输出查找到的内存分配信息
* 功   能:  在分配列表中查找是否存在同地址的分配信息
* 返回 值:  void
******************************************************************************/
void MemoryCheck::FindAndDeleteMemInfo(void * ptr, MemInfoT *& meminfo)
{
    assert(ptr != NULL);
    meminfo = NULL;
    //ns_lock::_Auto_Lock autolock(&_newlock);
    _not_trace_newlock.lock();
    MemInfoT * last = _not_trace_newlist;
    MemInfoT * curr = _not_trace_newlist;
    _not_trace_newlock.unlock();
    while (curr != NULL && curr->_ptr != ptr)
    {
        last = curr;
        curr = curr->_next;
    }

    //找到同地址的分配信息
    if (curr != NULL)
    {
        meminfo = curr;
        _not_trace_newlock.lock();
        if (curr == _not_trace_newlist)
        {
            _not_trace_newlist = curr->_next;
        }
        else
        {
            last->_next = curr->_next;
        }
        _not_trace_newlock.unlock();
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::DeleteMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  1、在内存分配链表中查找指定的地址的内存分配信息节点； 
            2、如果未找到指定地址的信息，则将内存释放信息插入释放链表
* 返回 值:  void
******************************************************************************/
int MemoryCheck::DeleteMemInfo(void * ptr, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
{
    assert(ptr != NULL);
    assert((type == OP_DELETE || type == OP_DELETE_ARRAY || type == OP_FREE));

    MemInfoT * curr = NULL;
    _new_hash_table.FindElement(ptr, curr);

    if (curr != NULL)  //找到匹配的地址
    {
        if (curr->_state == MEM_STATE_RELEASED || curr->_state == MEM_STATE_REPEATED_RELEASED 
            || curr->_state == MEM_STATE_DELETE_MISMATCH)
        {
            curr->_state = MEM_STATE_REPEATED_RELEASED; //置节点状态为重复释放
            
            MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type);
            temp._state = MEM_STATE_REPEATED_RELEASED; //置节点状态删除操作符不匹配
            InsertErrorInfo(temp);
        }
        else if (curr->_state == MEM_STATE_ALLOCATED)
        {
            curr->_state = MEM_STATE_RELEASED; //置当前节点状态为正常释放 
            if ((curr->_type == OP_NEW && type != OP_DELETE) ||
                (curr->_type == OP_NEW_ARRAY && type != OP_DELETE_ARRAY) ||
                (curr->_type == OP_MALLOC && type != OP_FREE))
            {
                curr->_state = MEM_STATE_DELETE_MISMATCH; //置当前节点状态为正常释放 
                //delete/Delete[]与new/new[]不匹配
                MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type, curr->_type);
                temp._state = MEM_STATE_DELETE_MISMATCH; //置节点状态删除操作符不匹配
                InsertErrorInfo(temp);
            }
            else
            {
                return 0; //正常释放内存
            }
        }
        else//正常情况下不存在这种场景
        {
            printf("[MemoryCheck][RELEASE_ERROR]DeleteMemInfo:Unknow state %d, addr:0x%08p, file:%s, func:%s, line:%u.\n",
                curr->_state, curr->_ptr, curr->_file, curr->_func, curr->_line);
        }
    }
    else  //不能找到地址
    {
        MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type);
        temp._state = MEM_STATE_UNKNOW_ADDR; //置节点状态为未找到地址
        InsertErrorInfo(temp);
    }
    return -1; //释放内存错误
}

/******************************************************************************
* 函数名称: MemoryCheck::InsertDeleteErrorInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  将释放内存出现错误时，将错误释放内存信息加入deletelist
* 返回 值:  void
******************************************************************************/
void MemoryCheck::InsertErrorInfo(MemInfoT meminfo)
{
    MemInfoT * errormeminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
    if (errormeminfo == NULL)
    {
        printf("[MemoryCheck][Malloc_Error]InsertDeleteErrorInfo, malloc error meminfo fail!.\n");
        return;
    }

    *errormeminfo = meminfo;

    ns_lock::_Auto_Lock autolock(&_errlock);
    errormeminfo->_next = _errorlist;
    _errorlist = errormeminfo;
}

/******************************************************************************
* 函数名称: MemoryCheck::DeleteMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  
           1、在内存分配链表中查找指定的地址的内存分配信息节点；
           2、如果未找到指定地址的信息，则将内存释放信息插入释放链表
* 返回 值:  void
******************************************************************************/
int MemoryCheck::DeleteMemInfo(void * ptr, MEM_OP_TYPE type)
{
    if (_not_trace_new_hash_table.FindAndDeleteElement(ptr) == 0)
    {
        return 0;
    }

    //printf("DeleteMemInfo %s, %s, %u.\n", top._file, top._func, top._line);
    return DeleteMemInfo(ptr, "", "", 0, type);
}

/******************************************************************************
* 函数名称: MemoryCheck::InsertMemInfo
* 作   者:  lijun
* 日   期:  2019.04.10
* 参   数:
* 功   能:
* 返回 值:  void
******************************************************************************/
int MemoryCheck::InsertMemInfo(void * ptr, size_t size, MEM_OP_TYPE type)
{
    assert(ptr != NULL);
    MemInfoT * meminfo = NULL;
    _not_trace_new_hash_table.FindElement(ptr, meminfo);

    if (meminfo == NULL)
    {
        MemInfoT temp = MemInfoT(ptr, size, "", "", 0, type, OP_NONE, MEM_STATE_ALLOCATED);
        _not_trace_new_hash_table.InsertElement(&temp);
    }
    else
    {
        printf("[MemoryCheck][Malloc_Error]InsertMemInfo, malloc mem repeat!.\n");
    }
    return 0;
}

/******************************************************************************
* 函数名称: MemoryCheck::FreeMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  释放内存检测存储的内存信息链表
* 返回 值:  void
******************************************************************************/
void MemoryCheck::FreeMemInfo(MemInfoT *& meminfo)
{
    MemInfoT * curr = meminfo;
    MemInfoT * del = curr;
    while (del != NULL)
    {
        curr = del->_next;
        free(del);
        del = curr;
    }
    meminfo = NULL;
}

/******************************************************************************
* 函数名称: MemoryCheck::FreeMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  释放内存检测存储的内存信息链表
* 返回 值:  void
******************************************************************************/
void MemoryCheck::FreeMemInfo()
{
    FreeMemInfo(_newlist);
    FreeMemInfo(_errorlist);
    FreeMemInfo(_not_trace_newlist);

    _new_hash_table.Reset();
    _not_trace_new_hash_table.Reset();
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintMemCheckResult
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印内存检测结果
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintMemCheckResult()
{
    PrintMemLeak();
    PrintRepeatedRelease();
    PrintReleaseTypeMismatch();
    PrintReleaseUnknowAddr();

    PrintReleaseSucceed();
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintReleaseTypeMismatch
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印释放类型不匹配
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintReleaseTypeMismatch()
{
    bool firstflag = true;
    MemInfoT * curr = _errorlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_DELETE_MISMATCH)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][RELEASE_MISMATCH]Find release type mismatch! as blow: \n");
                firstflag = false;
            }

            MEM_OP_TYPE expect_type = OP_DELETE;
            if (curr->_expect_type == OP_NEW)
            {
            }

            printf("[MemoryCheck][RELEASE_MISMATCH]Delete mismatch addr:%08p, release type:%s, but expect:%s, [file:%s, func:%s, line:%u].\n",
                curr->_ptr, OP_NAME[curr->_type],
                ((curr->_expect_type == OP_NEW) ? OP_NAME[OP_DELETE] : 
                (curr->_expect_type == OP_NEW_ARRAY) ? OP_NAME[OP_DELETE_ARRAY] : OP_NAME[OP_FREE]),
                curr->_file, curr->_func, curr->_line);
        }
        
        curr = curr->_next;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintReleaseUnknowAddr
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印释放未知地址 或 其它错误
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintReleaseUnknowAddr()
{
    bool firstflag = true;
    MemInfoT * curr = _errorlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_UNKNOW_ADDR)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release address unknow or other error! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][RELEASE_UNKNOW_ADDR]Memory addr:%08p, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_ptr, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }
        else if (curr->_state != MEM_STATE_DELETE_MISMATCH 
            && curr->_state != MEM_STATE_UNKNOW_ADDR
            && curr->_state != MEM_STATE_REPEATED_RELEASED)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release address unknow or other error! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][OTHER_ERROR]PrintReleaseUnknowAddr:Unknow state %d, addr:%08p, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_state, curr->_ptr, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }

        curr = curr->_next;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintMemLeak
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印未释放问题，泄漏
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintMemLeak()
{
    bool firstflag = true;
    _new_hash_table.ResetNextElement();
    MemInfoT * curr = NULL;
    while ((curr = _new_hash_table.NextElement()) != NULL)
    {
        if (curr->_state == MEM_STATE_ALLOCATED)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][MEMORY_LEAK]Find memory leak! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][MEMORY_LEAK]Memory addr:%08p, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_ptr, curr->_size, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }
   
        curr = curr->_next;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintRepeatedRelease
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印重复释放问题
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintRepeatedRelease()
{
    bool firstflag = true;
    MemInfoT * curr = _errorlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_REPEATED_RELEASED) //重复释放
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][REPEATED_RELEASE]Find repeated release memory! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][REPEATED_RELEASE]Memory addr:%08p, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_ptr, curr->_size, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }

        curr = curr->_next;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintReleaseSucceed
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印正确释放结果
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintReleaseSucceed()
{
    bool firstflag = true;
    _new_hash_table.ResetNextElement();
    MemInfoT * curr = NULL;
    while ((curr = _new_hash_table.NextElement()) != NULL)
    {
        if (curr->_state == MEM_STATE_RELEASED)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][RELEASE_SUCCEED]Release memory succeed! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][RELEASE_SUCCEED]Memory addr:%08p, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_ptr, curr->_size, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);

            //PrintBacktrace();
        }
        
        curr = curr->_next;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintBacktrace
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印函数调用栈
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintBacktrace()
{
#ifdef __linux__
#if 0
    void *buffer[20] = { 0 };
    size_t size = backtrace(buffer, 20);
    char **strings = backtrace_symbols(buffer, size);
    if (strings != NULL)
    {
        printf("Obtained %zd stack frames.\n", size);
        for (size_t i = 0; i < size; i++)
        {
            printf("%s\n", strings[i]);
        }

        free(strings);
        strings = NULL;
    }
#else
    void *bt[20]; 
    char **strings; 
    size_t sz;
    sz = backtrace(bt, 20); 
    strings = backtrace_symbols(bt, sz); 
    for(size_t i = 0; i < sz; ++i) 
    fprintf(stderr, "%s\n", strings[i]);
    free(strings);
#endif
#elif _WIN32
#if 1
    char * buf = (char *)malloc(1024);
    OPEN_STACK_TRACK(buf, 1024);
    printf(buf);
    free(buf);
#else
    TraceStack();
#endif
#endif
}

/******************************************************************************
* 函数名称: MemoryCheck::GetBacktrace
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  获取函数调用栈信息
* 返回 值:  void
******************************************************************************/
void MemoryCheck::GetBacktrace(char * buff, unsigned int maxlen)
{
#ifdef __linux__
    void *buffer[10] = { 0 };
    size_t size = backtrace(buffer, 10);
    char **strings = backtrace_symbols(buffer, size);
    if (strings != NULL)
    {
        printf("Obtained %zd stack frames.\n", size);
        for (size_t i = 0; i < size; i++)
        {
            printf("%s\n", strings[i]);
        }

        free(strings);
        strings = NULL;
    }
#elif _WIN32
#if 1
    OPEN_STACK_TRACK(buff, maxlen);
#else
    TraceStack();
#endif
#endif
}

/******************************************************************************
* 函数名称: MemoryCheck::SetDeleteInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  设置释放内存时的位置信息
* 返回 值:  void
******************************************************************************/
void MemoryCheck::SetDeleteInfo(const char * file, const char * func, unsigned int line)
{
    //与DeleteMemInfo的解锁对应，保证入栈和出栈的数据是同一个线程的，此锁时需要同一个线程可重入的
    GetDeleteLock();

    //printf("SetDeleteInfo %s, %s, %u.\n", file, func, line);
    bool bRet = _stack.Push(ElementT(0, file, func, line));
    assert(bRet);
}

/******************************************************************************
* 函数名称: MemoryCheck::SetNewInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  设置分配内存时的位置信息
* 返回 值:  void
******************************************************************************/
void MemoryCheck::SetNewInfo(const char * file, const char * func, unsigned int line)
{
    //与DeleteMemInfo的解锁对应，保证入栈和出栈的数据是同一个线程的，此锁时需要同一个线程可重入的
    GetDeleteLock();

    //printf("SetDeleteInfo %s, %s, %u.\n", file, func, line);
    bool bRet = _stack.Push(ElementT(0, file, func, line));
    assert(bRet);
}

/******************************************************************************
* 函数名称: MemoryCheck::GetDeleteLock
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  获取delete锁
* 返回 值:  void
******************************************************************************/
void MemoryCheck::GetDeleteLock()
{
    _deletelock.lock();
}

/******************************************************************************
* 函数名称: MemoryCheck::ReleaseDeleteLock
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  释放delete锁
* 返回 值:  void
******************************************************************************/
void MemoryCheck::ReleaseDeleteLock()
{
    _deletelock.unlock();
}
