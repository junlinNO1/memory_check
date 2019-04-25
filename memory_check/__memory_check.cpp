#include <iostream>
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

static const char * OP_NAME[] = { "new", "new[]", "delete", "delete[]" };

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
    if (ptr == NULL) 
        return;
    MemCheck.DeleteMemInfo(ptr, file, func, line, OP_DELETE);
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
    if (ptr == NULL)
        return;
    MemCheck.DeleteMemInfo(ptr, file, func, line, OP_DELETE_ARRAY);
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
void operator delete(void * ptr) throw()
{
    if (ptr == NULL)
        return;
    MemCheck.DeleteMemInfo(ptr, OP_DELETE);
}

/******************************************************************************
* 函数名称:  operator delete[]
* 作者:	 lijun
* 日期  2019.03.29
* 参数:
* 功能:  重载全局的 operator delete[]
* 返回值:  void
******************************************************************************/
void operator delete[](void * ptr) throw()
{
    if (ptr == NULL)
        return;
    MemCheck.DeleteMemInfo(ptr, OP_DELETE_ARRAY);
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
* 函数名称: MemoryCheck::SetDeleteInfo
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
* 函数名称: MemoryCheck::MemoryCheck
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  构造函数
* 返回 值:  void
******************************************************************************/

MemoryCheck::MemoryCheck() : newlist(NULL), errorlist(NULL)
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
* 返回 值:  void
******************************************************************************/
void MemoryCheck::InsertMemInfo(void * ptr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
{
    assert(ptr != NULL);
    MemInfoT * meminfo = NULL;
    ns_lock::_Auto_Lock autolock(&_newlock);
    FindMemInfo(ptr, meminfo);
    if (meminfo == NULL)
    {
        meminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
        if (meminfo == NULL)
        {
            printf("[MemoryCheck][Malloc_Error]InsertMemInfo, malloc meminfo fail!.\n");
            return;
        }

        *meminfo = MemInfoT(ptr, size, file, func, line, type);
        meminfo->_state = MEM_STATE_ALLOCATED;
        meminfo->_next = newlist;
        newlist = meminfo;
    }
    else
    {
        if (meminfo->_state != MEM_STATE_ALLOCATED && meminfo->_state != MEM_STATE_REPEAT_ALLOCATED)
        {
            //源节点已经释放过(无论当时释放是否出错)，则更新节点信息，重复利用节点
            *meminfo = MemInfoT(ptr, size, file, func, line, type);
            meminfo->_state = MEM_STATE_ALLOCATED;
        }
        else  //理论上不可能出现前一次未释放，而下一次分配在同一个堆地址上
        {
            printf("[MemoryCheck][Malloc_Error]InsertMemInfo, malloc mem repeat!.\n");
            MemInfoT temp = MemInfoT(ptr, size, file, func, line, type);
            temp._state = MEM_STATE_REPEAT_ALLOCATED; //置节点状态为重复分配
            InsertErrorInfo(temp);
        }
    }
    
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
    ns_lock::_Auto_Lock autolock(&_newlock);
    MemInfoT * curr = newlist;
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
* 函数名称: MemoryCheck::DeleteMemInfo
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  1、在内存分配链表中查找指定的地址的内存分配信息节点； 
            2、如果未找到指定地址的信息，则将内存释放信息插入释放链表
* 返回 值:  void
******************************************************************************/
void MemoryCheck::DeleteMemInfo(void * ptr, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
{
    assert(ptr != NULL);
    assert((type == OP_DELETE || type == OP_DELETE_ARRAY));

    ns_lock::_Auto_Lock autolock(&_newlock);

    MemInfoT * curr = newlist;
    MemInfoT * last = NULL;
    while (curr != NULL && curr->_ptr != ptr)
    {
        last = curr;
        curr = curr->_next;
    }

    if (curr != NULL)  //找到匹配的地址
    {
        if (curr->_state == MEM_STATE_RELEASED || curr->_state == MEM_STATE_REPEAT_RELEASED 
            || curr->_state == MEM_STATE_DELETE_NOT_MATCH)
        {
            curr->_state = MEM_STATE_REPEAT_RELEASED; //置节点状态为重复释放
            
            MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type);
            temp._state = MEM_STATE_REPEAT_RELEASED; //置节点状态删除操作符不匹配
            InsertErrorInfo(temp);
        }
        else if (curr->_state == MEM_STATE_ALLOCATED)
        {
            curr->_state = MEM_STATE_RELEASED; //置当前节点状态为正常释放 
            if (curr->_type != (type - OP_DELETE))
            {
                curr->_state = MEM_STATE_DELETE_NOT_MATCH; //置当前节点状态为正常释放 
                //delete/Delete[]与new/new[]不匹配
                MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type);
                temp._state = MEM_STATE_DELETE_NOT_MATCH; //置节点状态删除操作符不匹配
                InsertErrorInfo(temp);
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
    errormeminfo->_next = errorlist;
    errorlist = errormeminfo;
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
void MemoryCheck::DeleteMemInfo(void * ptr, MEM_OP_TYPE type)
{
    ElementT top;
    bool bRet = _stack.Pop(top);
    assert(bRet);
    
    //与SetDeleteInfo的加锁对应，保证入栈和出栈的数据是同一个线程的，此锁时需要同一个线程可重入的
    _deletelock.unlock();  

    //printf("DeleteMemInfo %s, %s, %u.\n", top._file, top._func, top._line);
    DeleteMemInfo(ptr, top._file, top._func, top._line, type);
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
    FreeMemInfo(newlist);
    FreeMemInfo(errorlist);
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
    PrintRepeatRelease();
    PrintReleaseTypeNotMatch();
    PrintReleaseUnknowAddr();

    PrintReleaseSucceed();
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintReleaseTypeNotMatch
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印释放类型不匹配
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintReleaseTypeNotMatch()
{
    bool firstflag = true;
    MemInfoT * curr = errorlist;
    while (curr != NULL)
    {
        if (firstflag)
        {
            printf("------------------------------------------------------------------- \n");
            printf("[MemoryCheck][RELEASE_NOT_MATCH]Find release type not match! as blow: \n");
            firstflag = false;
        }

        if (curr->_state == MEM_STATE_DELETE_NOT_MATCH)
        {
            printf("[MemoryCheck][RELEASE_NOT_MATCH]Delete addr:0x%08p, use:%s, but expect:%s, not match, please check! file:%s, func:%s, line:%u.\n",
                curr->_ptr, ((curr->_type == OP_NEW) ? OP_NAME[OP_DELETE_ARRAY] : OP_NAME[OP_DELETE]),
                ((curr->_type == OP_NEW) ? OP_NAME[OP_DELETE] : OP_NAME[OP_DELETE_ARRAY]),
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
    MemInfoT * curr = errorlist;
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

            printf("[MemoryCheck][RELEASE_UNKNOW_ADDR]Memory addr:0x%08p, file:%s, func:%s, line:%u.\n",
                curr->_ptr, curr->_file, curr->_func, curr->_line);
        }
        else if (curr->_state != MEM_STATE_DELETE_NOT_MATCH 
            && curr->_state != MEM_STATE_UNKNOW_ADDR
            && curr->_state != MEM_STATE_REPEAT_RELEASED)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release address unknow or other error! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][OTHER_ERROR]PrintReleaseUnknowAddr:Unknow state %d, addr:0x%08p, file:%s, func:%s, line:%u.\n",
                curr->_state, curr->_ptr, curr->_file, curr->_func, curr->_line);
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
    MemInfoT * curr = newlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_ALLOCATED)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][MEMORY_LEAK]Find memory leak! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][MEMORY_LEAK]Memory addr:0x%08p, size:%u, use:%s, file:%s, func:%s, line:%u.\n",
                curr->_ptr, curr->_size, ((curr->_type == OP_NEW) ? OP_NAME[OP_NEW] : OP_NAME[OP_NEW_ARRAY]),
                curr->_file, curr->_func, curr->_line);
        }
   
        curr = curr->_next;
    }
}

/******************************************************************************
* 函数名称: MemoryCheck::PrintRepeatRelease
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  打印重复释放问题
* 返回 值:  void
******************************************************************************/
void MemoryCheck::PrintRepeatRelease()
{
    bool firstflag = true;
    MemInfoT * curr = errorlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_REPEAT_RELEASED) //重复释放
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][REPEAT_RELEASE]Find repeat release memory! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][REPEAT_RELEASE]Memory addr:0x%08p, size:%u, use:%s, file:%s, func:%s, line:%u.\n",
                curr->_ptr, curr->_size, ((curr->_type == OP_NEW) ? OP_NAME[OP_NEW] : OP_NAME[OP_NEW_ARRAY]),
                curr->_file, curr->_func, curr->_line);
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
    MemInfoT * curr = newlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_RELEASED)
        {
            if (firstflag)
            {
                printf("------------------------------------------------------------------- \n");
                printf("[MemoryCheck][RELEASE_SUCCEED]Release memory succeed! as blow: \n");
                firstflag = false;
            }

            printf("[MemoryCheck][RELEASE_SUCCEED]Memory addr:0x%08p, size:%u, use:%s, file:%s, func:%s, line:%u.\n",
                curr->_ptr, curr->_size, ((curr->_type == OP_NEW) ? OP_NAME[OP_NEW] : OP_NAME[OP_NEW_ARRAY]),
                curr->_file, curr->_func, curr->_line);

            PrintBacktrace();
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
    _deletelock.lock();

    //printf("SetDeleteInfo %s, %s, %u.\n", file, func, line);
    bool bRet = _stack.Push(ElementT(file, func, line));
    assert(bRet);
}
