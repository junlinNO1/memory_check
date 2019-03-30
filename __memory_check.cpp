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

#if 0
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
void operator delete(void * ptr)
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
void operator delete[](void * ptr)
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
    MemCheck.PrintMemLeak();
    MemCheck.PrintRepeatRelease();
    MemCheck.PrintReleaseTypeNotMatch();
    MemCheck.PrintReleaseUnknowAddr();

    MemCheck.PrintReleaseSucceed();
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
* 函数名称: MemoryCheck::MemoryCheck
* 作   者:  lijun
* 日   期:  2019.03.29
* 参   数:
* 功   能:  构造函数
* 返回 值:  void
******************************************************************************/
MemoryCheck::MemoryCheck() : newlist(NULL), deletelist(NULL)
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
    FindMemInfo(ptr, meminfo);
    if (meminfo == NULL)
    {
        meminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
        if (meminfo == NULL)
        {
            printf("[MemoryCheck][Malloc_Error]InsertMemInfo, malloc new meminfo fail!.\n");
            return;
        }

        *meminfo = MemInfoT(ptr, size, file, func, line, type);
        meminfo->_state = MEM_STATE_ALLOCATED;
        meminfo->_next = newlist;
        newlist = meminfo;
    }
    else
    {
        if (meminfo->_state == MEM_STATE_RELEASED)
        {
            *meminfo = MemInfoT(ptr, size, file, func, line, type);
            meminfo->_state = MEM_STATE_ALLOCATED;
        }
        else
        {

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
    MemInfoT * curr = newlist;
    MemInfoT * last = NULL;
    while (curr != NULL && curr->_ptr != ptr)
    {
        last = curr;
        curr = curr->_next;
    }

    if (curr != NULL)  //找到匹配的地址
    {
        if (curr->_state == MEM_STATE_RELEASED || curr->_state == MEM_STATE_REPEAT_RELEASED)
        {
            curr->_state = MEM_STATE_REPEAT_RELEASED; //置节点状态为重复释放
        }
        else if (curr->_state == MEM_STATE_ALLOCATED)
        {
            if (curr->_type == (type - OP_DELETE))
            {
                curr->_state = MEM_STATE_RELEASED; //置节点状态为正常释放                
            }
            else
            {
                //delete/Delete[]与new/new[]不匹配
                MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type);
                temp._state = MEM_STATE_DELETE_NOT_MATCH; //置节点状态删除操作符不匹配
                InsertDeleteErrorInfo(temp);

                MemInfoT * deletememinfo = (MemInfoT *)malloc(sizeof(MemInfoT));
                if (deletememinfo == NULL)
                {
                    printf("[MemoryCheck][Malloc_Error]DeleteMemInfo, malloc delete meminfo fail!.\n");
                    return;
                }

                *deletememinfo = MemInfoT(ptr, 0, file, func, line, type);
                deletememinfo->_state = MEM_STATE_DELETE_NOT_MATCH; //置节点状态删除操作符不匹配
                deletememinfo->_next = deletelist;
                deletelist = deletememinfo;
            }
        }
        else//正常情况下不存在这种场景
        {
            printf("[MemoryCheck][RELEASE_ERROR]DeleteMemInfo:Unknow state %d, addr:0x%p, file:%s, func:%s, line:%u.\n",
                curr->_state, curr->_ptr, curr->_file, curr->_func, curr->_line);
        }
    }
    else  //不能找到地址
    {
        MemInfoT temp = MemInfoT(ptr, 0, file, func, line, type);
        temp._state = MEM_STATE_UNKNOW_ADDR; //置节点状态为未找到地址
        InsertDeleteErrorInfo(temp);
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
void MemoryCheck::InsertDeleteErrorInfo(MemInfoT meminfo)
{
    MemInfoT * deletememinfo = (MemInfoT *)malloc(sizeof(MemInfoT));
    if (deletememinfo == NULL)
    {
        printf("[MemoryCheck][Malloc_Error]InsertDeleteErrorInfo, malloc delete meminfo fail!.\n");
        return;
    }

    *deletememinfo = meminfo;
    deletememinfo->_next = deletelist;
    deletelist = deletememinfo;
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
    DeleteMemInfo(ptr, NULL, NULL, 0, type);
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
    FreeMemInfo(deletelist);
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
    MemInfoT * curr = deletelist;
    while (curr != NULL)
    {
        if (firstflag)
        {
            printf("\n\n[MemoryCheck][RELEASE_NOT_MATCH]Find release type not match! as blow:-------------------------------------------------------- \n");
            firstflag = false;
        }

        if (curr->_state == MEM_STATE_DELETE_NOT_MATCH)
        {
            printf("[MemoryCheck][RELEASE_NOT_MATCH]Delete addr:0x%p, use:%s, but expect:%s, not match, please check! ---- file:%s, func:%s, line:%u.\n",
                curr->_ptr, ((curr->_type == OP_NEW) ? OP_NAME[OP_DELETE_ARRAY] : OP_NAME[OP_DELETE]),
                ((curr->_type == OP_NEW) ? OP_NAME[OP_DELETE] : OP_NAME[OP_DELETE_ARRAY]),
                curr->_file, curr->_func, curr->_line);
            PrintBacktrace();
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
    MemInfoT * curr = deletelist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_UNKNOW_ADDR)
        {
            if (firstflag)
            {
                printf("\n\n[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release type not match or other error! as blow:-------------------------------------------------------- \n");
                firstflag = false;
            }

            printf("[MemoryCheck][RELEASE_UNKNOW_ADDR]Memory addr:0x%p, file:%s, func:%s, line:%u.\n",
                curr->_ptr, curr->_file, curr->_func, curr->_line);

            //PrintBacktrace();
        }
        else if (curr->_state != MEM_STATE_DELETE_NOT_MATCH && curr->_state != MEM_STATE_UNKNOW_ADDR)
        {
            if (firstflag)
            {
                printf("\n\n[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release type not match or other error! as blow:-------------------------------------------------------- \n");
                firstflag = false;
            }

            printf("[MemoryCheck][RELEASE_ERROR]PrintReleaseTypeNotMatch:Unknow state %d, addr:0x%p, file:%s, func:%s, line:%u.\n",
                curr->_state, curr->_ptr, curr->_file, curr->_func, curr->_line);

            //PrintBacktrace();
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
        if (curr->_state != MEM_STATE_RELEASED && curr->_state != MEM_STATE_REPEAT_RELEASED)
        {
            if (firstflag)
            {
                printf("\n\n[MemoryCheck][MEMORY_LEAK]Find memory leak! as blow:-------------------------------------------------------- \n");
                firstflag = false;
            }

            printf("[MemoryCheck][MEMORY_LEAK]Memory addr:0x%p, size:%u, use:%s, file:%s, func:%s, line:%u.\n",
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
    MemInfoT * curr = newlist;
    while (curr != NULL)
    {
        if (curr->_state == MEM_STATE_REPEAT_RELEASED) //重复释放
        {
            if (firstflag)
            {
                printf("\n\n[MemoryCheck][REPEAT_RELEASE]Find repeat release memory! as blow:-------------------------------------------------------- \n");
                firstflag = false;
            }

            printf("[MemoryCheck][REPEAT_RELEASE]Memory addr:0x%p, size:%u, use:%s, file:%s, func:%s, line:%u.\n",
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
                printf("\n\n[MemoryCheck][RELEASE_SUCCEED]Release memory succeed! as blow:-------------------------------------------------------- \n");
                firstflag = false;
            }

            printf("[MemoryCheck][RELEASE_SUCCEED]Memory addr:0x%p, size:%u, use:%s, file:%s, func:%s, line:%u.\n",
                curr->_ptr, curr->_size, ((curr->_type == OP_NEW) ? OP_NAME[OP_NEW] : OP_NAME[OP_NEW_ARRAY]),
                curr->_file, curr->_func, curr->_line);
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
    void *buffer[100];
    char **strings;

    int nptrs = backtrace(buffer, 100);
    printf("backtrace() returned %d addresses\n", nptrs);

    backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);

    for (int i = 0; i < sz; ++i)
    {
        fprintf(stderr, "%s\n", strings[i]);
    }
#elif _WIN32
#if 1
    OPEN_STACK_TRACK
#else
    TraceStack();
#endif
#endif
}




