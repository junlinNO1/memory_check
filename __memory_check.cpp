#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
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
const int CORRECT_WRITE = 1;
const int INCORRECT_WRITE = 0;

//全局内存检测对象
static MemoryCheck MemCheck;



/*************************************************************************************
* 函数名称: GetNowTime
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能: 获取当前时间
* 返 回 值: void
*************************************************************************************/
void GetNowTime(char * timebuf, int len)
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);  //获取相对于1970到现在的秒数

    struct tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);

    memset(timebuf, 0, len);
    sprintf(timebuf, "%04d/%02d/%02d %02d:%02d:%02d", nowTime.tm_year + 1900, nowTime.tm_mon+1, nowTime.tm_mday, 
      nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);
}

/*************************************************************************************
* 函数名称: operator new
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator new, 用于记录分配内存的位置等信息
* 返 回 值: void
*************************************************************************************/
void * operator new(size_t size, const char * file, const char * func, unsigned int line)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(reinterpret_cast<unsigned long>(ptr), size, file, func, line, OP_NEW);
    return ptr;
}

/*************************************************************************************
* 函数名称: operator new[]
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator new[], 用于记录分配内存的位置等信息
* 返 回 值: void
*************************************************************************************/
void * operator new[](size_t size, const char * file, const char * func, unsigned int line)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(reinterpret_cast<unsigned long>(ptr), size, file, func, line, OP_NEW_ARRAY);
    return ptr;
}

/*************************************************************************************
* 函数名称: operator new
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator new, 用于记录分配内存的位置等信息
* 返 回 值: void
*************************************************************************************/
void * operator new(size_t size)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(reinterpret_cast<unsigned long>(ptr), size, OP_NEW);
    return ptr;
}

/*************************************************************************************
* 函数名称: operator new[]
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator new[], 用于记录分配内存的位置等信息
* 返 回 值: void
*************************************************************************************/
void * operator new[](size_t size)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(reinterpret_cast<unsigned long>(ptr), size, OP_NEW_ARRAY);
    return ptr;
}

/*************************************************************************************
* 函数名称:     operator delete
* 作     者:	lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator delete
* 返 回 值: void
*************************************************************************************/
void operator delete(void * ptr, const char * file, const char * func, unsigned int line)
{
    printf("[MemoryCheck][Malloc_Error]operator delete(void * ptr, const char * file, const char * func, unsigned int line)!.\n");
    if (ptr == NULL) 
        return;

    if (0 == MemCheck.DeleteMemInfo(reinterpret_cast<unsigned long>(ptr), file, func, line, OP_DELETE))
    {
        free(ptr);
    }
}

/*************************************************************************************
* 函数名称:     operator delete
* 作     者:	lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator delete
* 返 回 值: void
*************************************************************************************/
void operator delete[](void * ptr, const char * file, const char * func, unsigned int line)
{
    printf("[MemoryCheck][Malloc_Error]operator delete[](void * ptr, const char * file, const char * func, unsigned int line)!.\n");
    if (ptr == NULL)
        return;

    if (0 == MemCheck.DeleteMemInfo(reinterpret_cast<unsigned long>(ptr), file, func, line, OP_DELETE_ARRAY))
    {
        free(ptr);
    }
}

/*************************************************************************************
* 函数名称:     operator delete
* 作     者: lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator delete
* 返 回 值: void
*************************************************************************************/
void operator delete(void * ptr) throw()
{
    if (ptr == NULL)
        return;

    if (0 == MemCheck.DeleteMemInfo(reinterpret_cast<unsigned long>(ptr), OP_DELETE))
    {
    	free(ptr);
    }
}

/*************************************************************************************
* 函数名称:     operator delete[]
* 作     者:	lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  重载全局的 operator delete[]
* 返 回 值: void
*************************************************************************************/
void operator delete[](void * ptr) throw()
{
    if (ptr == NULL)
        return;

    if (0 == MemCheck.DeleteMemInfo(reinterpret_cast<unsigned long>(ptr), OP_DELETE_ARRAY))
    {
    	free(ptr);
    }
}

/*************************************************************************************
* 函数名称: malloc_memory_check
* 作     者:  lijun
* 日     期:  2019.04.10
* 参     数:
* 功     能:  模拟malloc分配内存
* 返 回 值: void
*************************************************************************************/
void * malloc_memory_check(size_t size, const char * file, const char * func, unsigned int line)
{
    void * ptr = malloc(size);
    if (ptr == NULL)
        return NULL;

    MemCheck.InsertMemInfo(reinterpret_cast<unsigned long>(ptr), size, file, func, line, OP_MALLOC);
    return ptr;
}

/*************************************************************************************
* 函数名称: free_memory_check
* 作     者:  lijun
* 日     期:  2019.04.10
* 参     数:
* 功     能:  模拟free释放内存
* 返 回 值: void
*************************************************************************************/
void free_memory_check(void * ptr, const char * file, const char * func, unsigned int line)
{
    if (ptr == NULL)
        return;

    if (0 == MemCheck.DeleteMemInfo(reinterpret_cast<unsigned long>(ptr), file, func, line, OP_FREE))
    {
        free(ptr);
    }
}

/*************************************************************************************
* 函数名称:     PrintMemCheckResult
* 作     者:	lijun
* 日     期   2019.03.29
* 参     数:
* 功     能:  全局对外接口，打印内存检测结果
* 返 回 值: void
*************************************************************************************/
void PrintMemCheckResult()
{
    MemCheck.PrintMemCheckResult();
}

/*************************************************************************************
* 函数名称: FreeMemCheck
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  全局对外接口，打印内存检测结果
* 返 回 值: void
*************************************************************************************/
void FreeMemCheck()
{
    MemCheck.FreeMemInfo();
}

/*************************************************************************************
* 函数名称: FreeNextElment
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能: 用于释放NextElment的回调函数，注册给hashtable
* 返 回 值: void
*************************************************************************************/
void FreeNextElment(NextElemT * elem)
{
	if (elem == NULL ) return;
	free(elem);
}

/*************************************************************************************
* 函数名称: MemoryCheck::MemoryCheck
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  构造函数
* 返 回 值: void
*************************************************************************************/
MemoryCheck::MemoryCheck() 
	: _errorlist(NULL)
	, _correct_released_file(NULL)
	, _incorrect_released_file(NULL)
	, _incorrect_released_bt(NULL)
{
    _new_hash_table.RegesiterReleaseFunc(FreeNextElment);
    _not_trace_new_hash_table.RegesiterReleaseFunc(FreeNextElment);
}

/*************************************************************************************
* 函数名称: MemoryCheck::~MemoryCheck
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  析构函数，释放内存分配信息链表
* 返 回 值: void
*************************************************************************************/
MemoryCheck::~MemoryCheck()
{
    FreeMemInfo();
}

/*************************************************************************************
* 函数名称: MemoryCheck::InsertMemInfo
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  将内存分配信息插入分配链表
* 返 回 值: int
*************************************************************************************/
int MemoryCheck::InsertMemInfo(unsigned long addr, unsigned int size, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
{
    NextElemT * nextelem = NULL;
    _new_hash_table.FindElement(addr, nextelem);
	MemInfoT * meminfo = (MemInfoT *)nextelem;

    if (meminfo == NULL)
    {
    	meminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
		if (meminfo == NULL) 
		{
			printf("[MemoryCheck][ERROR]InsertMemInfo malloc fail!\n");
			return -1;
		}

        *meminfo = MemInfoT(addr, size, file, func, line, type, OP_NONE, MEM_STATE_ALLOCATED, NULL);
        _new_hash_table.InsertElement(addr, &(meminfo->_next));
    }
    else
    {
        if (meminfo->_state != MEM_STATE_ALLOCATED && meminfo->_state != MEM_STATE_REPEAT_ALLOCATED)
        {
            //源节点已经释放过(无论当时释放是否出错)，则更新节点信息，重复利用节点
            WriteToFile(CORRECT_WRITE, "[MemoryCheck][RELEASE_SUCCEED][REPEAT]Memory addr:0x%08x, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                meminfo->_next._real_key, meminfo->_size, OP_NAME[meminfo->_type], meminfo->_file, meminfo->_func, meminfo->_line);
                
            *meminfo = MemInfoT(addr, size, file, func, line, type, OP_NONE, MEM_STATE_ALLOCATED, meminfo->_next._next);
            //printf("[MemoryCheck][MEM_STATE_REPEAT_ALLOCATED] repeat 0x%08x!.\n", addr);
        }
        else  //理论上不可能出现前一次未释放，而下一次分配在同一个堆地址上
        {
            printf("[MemoryCheck][Malloc_Error]InsertMemInfo, malloc mem repeat!.\n");
            MemInfoT temp = MemInfoT(addr, size, file, func, line, type);
            temp._state = MEM_STATE_REPEAT_ALLOCATED; //置节点状态为重复分配
            InsertErrorInfo(temp);
        }
    }
    return 0;
}

/*************************************************************************************
* 函数名称: MemoryCheck::DeleteMemInfo
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  1、在内存分配链表中查找指定的地址的内存分配信息节点； 
            2、如果未找到指定地址的信息，则将内存释放信息插入释放链表
* 返 回 值: void
*************************************************************************************/
int MemoryCheck::DeleteMemInfo(unsigned long addr, const char * file, const char * func, unsigned int line, MEM_OP_TYPE type)
{
    assert((type == OP_DELETE || type == OP_DELETE_ARRAY || type == OP_FREE));

    NextElemT * nextelem = NULL;
    _new_hash_table.FindElement(addr, nextelem);

	MemInfoT * find_info = (MemInfoT *)nextelem;
    if (find_info != NULL)  //找到匹配的地址
    {
        if (find_info->_state == MEM_STATE_RELEASED || find_info->_state == MEM_STATE_REPEATED_RELEASED 
            || find_info->_state == MEM_STATE_DELETE_MISMATCH)
        {
            find_info->_state = MEM_STATE_REPEATED_RELEASED; //置节点状态为重复释放

            MemInfoT temp;
            if (line == 0) {
            	temp = MemInfoT(addr, 0, find_info->_file, find_info->_func, find_info->_line, type);
            }
            else {
            	temp = MemInfoT(addr, 0, file, func, line, type);
            }
            temp._state = MEM_STATE_REPEATED_RELEASED; 
            InsertErrorInfo(temp);

            PrintBacktrace("\n[MemoryCheck][REPEATED_RELEASE]Memory addr:0x%08x, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                temp._next._real_key, temp._size, OP_NAME[temp._type], temp._file, temp._func, temp._line);
        }
        else if (find_info->_state == MEM_STATE_ALLOCATED)
        {
            find_info->_state = MEM_STATE_RELEASED; //置当前节点状态为正常释放 
            if ((find_info->_type == OP_NEW && type != OP_DELETE) ||
                (find_info->_type == OP_NEW_ARRAY && type != OP_DELETE_ARRAY) ||
                (find_info->_type == OP_MALLOC && type != OP_FREE))
            {
                find_info->_state = MEM_STATE_DELETE_MISMATCH;  //置节点状态删除操作符不匹配
                //delete/Delete[]与new/new[]不匹配
                MemInfoT temp;
                if (line == 0) {
                	temp = MemInfoT(addr, 0, find_info->_file, find_info->_func, find_info->_line, type, find_info->_type);
                }
                else {
                	temp = MemInfoT(addr, 0, file, func, line, type, find_info->_type);
                }
                temp._state = MEM_STATE_DELETE_MISMATCH; 
                InsertErrorInfo(temp);

                PrintBacktrace("\n[MemoryCheck][RELEASE_MISMATCH]Delete mismatch addr:0x%08x, release type:%s, but expect:%s, [file:%s, func:%s, line:%u].\n",
	                temp._next._real_key, OP_NAME[temp._type],
	                ((temp._expect_type == OP_NEW) ? OP_NAME[OP_DELETE] : 
	                (temp._expect_type == OP_NEW_ARRAY) ? OP_NAME[OP_DELETE_ARRAY] : OP_NAME[OP_FREE]),
	                temp._file, temp._func, temp._line);
            }
            else
            {
            	//printf("[MemoryCheck][RELEASE_SUCCESS][0x%08x]\n", addr);
                return 0; //正常释放内存
            }
        }
        else//正常情况下不存在这种场景
        {
            printf("[MemoryCheck][RELEASE_ERROR]DeleteMemInfo:Unknow state %d, addr:0x%08x, file:%s, func:%s, line:%u.\n",
                find_info->_state, find_info->_next._real_key, find_info->_file, find_info->_func, find_info->_line);
        }
    }
    else  //不能找到地址
    {
        MemInfoT temp = MemInfoT(addr, 0, file, func, line, type);
        temp._state = MEM_STATE_UNKNOW_ADDR; //置节点状态为未找到地址
        InsertErrorInfo(temp);

        PrintBacktrace("\n[MemoryCheck][RELEASE_UNKNOW_ADDR]Memory addr:0x%08x, release type:%s, [file:%s, func:%s, line:%u].\n",
        	temp._next._real_key, OP_NAME[temp._type], temp._file, temp._func, temp._line);
    }
    return -1; //释放内存错误
}

/*************************************************************************************
* 函数名称: MemoryCheck::InsertDeleteErrorInfo
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: MemInfoT&    -- 错误信息节点
* 功     能:  将释放内存出现错误时，将错误释放内存信息加入deletelist
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::InsertErrorInfo(MemInfoT & meminfo)
{
    MemInfoT * errormeminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
    if (errormeminfo == NULL)
    {
        printf("[MemoryCheck][ERROR]InsertDeleteErrorInfo, malloc error meminfo fail!.\n");
        return;
    }

    *errormeminfo = meminfo;

    ns_lock::_Auto_Lock autolock(&_errlock);
    errormeminfo->_next._next = (NextElemT*)_errorlist;
    _errorlist = errormeminfo;
}

/*************************************************************************************
* 函数名称: MemoryCheck::DeleteMemInfo
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: unsigned long     --  内存地址
            MEM_OP_TYPE      --  内存操作类型（delete/delete[]/free）
* 功     能:  1、在内存分配链表中查找指定的地址的内存分配信息节点；
            2、如果未找到指定地址的信息，则将内存释放信息插入释放链表

            注意：由于无法区分当前的delete 是否是需要检测的分配对应的delete，
            因此，首先尝试在_not_trace_new_hash_table中查找是否存在指定地址的分配信息，
            如果存在，则直接从_not_trace_new_hash_table中删除，不做校验。
            如果不存在，则在_new_hash_table中查找是否是需要检测的。
* 返 回 值: void
*************************************************************************************/
int MemoryCheck::DeleteMemInfo(unsigned long addr, MEM_OP_TYPE type)
{
    if (_not_trace_new_hash_table.FindAndDeleteElement(addr) == 0)
    {
        return 0;
    }

    return DeleteMemInfo(addr, "", "", 0, type);
}

/*************************************************************************************
* 函数名称: MemoryCheck::InsertMemInfo
* 作     者:  lijun
* 日     期:  2019.04.10
* 参     数:
* 功     能: 用于存储由全局作用域范围内的new分配的内存信息，此部分并非内存检测需要，
            所以单独存储在_not_trace_new_hash_table表中
* 返 回 值: void
*************************************************************************************/
int MemoryCheck::InsertMemInfo(unsigned long addr, size_t size, MEM_OP_TYPE type)
{
    NextElemT * next = NULL;
    _not_trace_new_hash_table.FindElement(addr, next);
    MemInfoT * meminfo = (MemInfoT *)next;

    if (meminfo == NULL)
    {
        meminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
		if (meminfo == NULL) 
		{
			printf("[MemoryCheck][ERROR]InsertMemInfo malloc fail!\n");
			return -1;
		}

        *meminfo = MemInfoT(addr, size, "", "", 0, type, OP_NONE, MEM_STATE_ALLOCATED, NULL);
        _not_trace_new_hash_table.InsertElement(addr, &(meminfo->_next));
    }
    else
    {
        printf("[MemoryCheck][ERROR]InsertMemInfo, address repeat!.\n");
    }
    return 0;
}

/*************************************************************************************
* 函数名称: MemoryCheck::FreeMemInfo
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: MemInfoT *&  -- 内存信息链表
* 功     能:  释放内存检测存储的内存信息链表
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::FreeMemInfo(MemInfoT *& meminfo)
{
    MemInfoT * curr = meminfo;
    MemInfoT * del = curr;
    while (del != NULL)
    {
        curr = (MemInfoT *)del->_next._next;
        free(del);
        del = curr;
    }
    meminfo = NULL;
}

/*************************************************************************************
* 函数名称: MemoryCheck::FreeMemInfo
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  释放内存检测存储的内存信息链表
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::FreeMemInfo()
{
	if (_correct_released_file != NULL)
	{
		fclose(_correct_released_file);
		_correct_released_file = NULL;
	}

	if (_incorrect_released_file != NULL)
	{
		fclose(_incorrect_released_file);
		_incorrect_released_file = NULL;
	}

	if (_incorrect_released_bt != NULL)
	{
		fclose(_incorrect_released_bt);
		_incorrect_released_bt = NULL;
	}

    FreeMemInfo(_errorlist);

    _new_hash_table.ResetTable();
    _not_trace_new_hash_table.ResetTable();
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintMemCheckResult
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能:  打印内存检测结果
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::PrintMemCheckResult()
{
	_new_hash_table.PrintHashScoredHit();

    PrintMemLeak();
    PrintRepeatedRelease();
    PrintReleaseTypeMismatch();
    PrintReleaseUnknowAddr();

    PrintReleaseSucceed();
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintReleaseTypeMismatch
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数: void
* 功     能:  打印释放类型不匹配
* 返 回 值: void
*************************************************************************************/
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
                WriteToFile(INCORRECT_WRITE, "------------------------------------------------------------------- \n");
                WriteToFile(INCORRECT_WRITE, "[MemoryCheck][RELEASE_MISMATCH]Find release type mismatch! as blow: \n");
                firstflag = false;
            }

            WriteToFile(INCORRECT_WRITE, "[MemoryCheck][RELEASE_MISMATCH]Delete mismatch addr:0x%08x, release type:%s, but expect:%s, [file:%s, func:%s, line:%u].\n",
                curr->_next._real_key, OP_NAME[curr->_type],
                ((curr->_expect_type == OP_NEW) ? OP_NAME[OP_DELETE] : 
                (curr->_expect_type == OP_NEW_ARRAY) ? OP_NAME[OP_DELETE_ARRAY] : OP_NAME[OP_FREE]),
                curr->_file, curr->_func, curr->_line);
        }
        
        curr = (MemInfoT * )curr->_next._next;
    }
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintReleaseUnknowAddr
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  打印释放未知地址 或 其它错误
* 返 回 值: void
*************************************************************************************/
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
                WriteToFile(INCORRECT_WRITE, "------------------------------------------------------------------- \n");
                WriteToFile(INCORRECT_WRITE, "[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release address unknow or other error! as blow: \n");
                firstflag = false;
            }

            WriteToFile(INCORRECT_WRITE, "[MemoryCheck][RELEASE_UNKNOW_ADDR]Memory addr:0x%08x, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_next._real_key, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }
        else if (curr->_state != MEM_STATE_DELETE_MISMATCH 
            && curr->_state != MEM_STATE_UNKNOW_ADDR
            && curr->_state != MEM_STATE_REPEATED_RELEASED)
        {
            if (firstflag)
            {
                WriteToFile(INCORRECT_WRITE, "------------------------------------------------------------------- \n");
                WriteToFile(INCORRECT_WRITE, "[MemoryCheck][RELEASE_UNKNOW_ADDR]Find release address unknow or other error! as blow: \n");
                firstflag = false;
            }

            WriteToFile(INCORRECT_WRITE, "[MemoryCheck][OTHER_ERROR]PrintReleaseUnknowAddr:Unknow state %d, addr:0x%08x, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_state, curr->_next._real_key, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }

        curr = (MemInfoT * )curr->_next._next;
    }
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintMemLeak
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  打印未释放问题，泄漏
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::PrintMemLeak()
{
    bool firstflag = true;
    _new_hash_table.InitForNextElement();
    NextElemT * nextelem = NULL;
    while ((nextelem = _new_hash_table.NextElement()) != NULL)
    {
    	MemInfoT * curr = (MemInfoT *)nextelem;
        if (curr->_state == MEM_STATE_ALLOCATED)
        {
            if (firstflag)
            {
                WriteToFile(INCORRECT_WRITE, "------------------------------------------------------------------- \n");
                WriteToFile(INCORRECT_WRITE, "[MemoryCheck][MEMORY_LEAK]Find memory leak! as blow: \n");
                firstflag = false;
            }

            WriteToFile(INCORRECT_WRITE, "[MemoryCheck][MEMORY_LEAK]Memory addr:0x%08x, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_next._real_key, curr->_size, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }
    }
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintRepeatedRelease
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  打印重复释放问题
* 返 回 值: void
*************************************************************************************/
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
                WriteToFile(INCORRECT_WRITE, "------------------------------------------------------------------- \n");
                WriteToFile(INCORRECT_WRITE, "[MemoryCheck][REPEATED_RELEASE]Find repeated release memory! as blow: \n");
                firstflag = false;
            }

            WriteToFile(INCORRECT_WRITE, "[MemoryCheck][REPEATED_RELEASE]Memory addr:0x%08x, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_next._real_key, curr->_size, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }

        curr = (MemInfoT * )curr->_next._next;
    }
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintReleaseSucceed
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  打印正确释放结果
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::PrintReleaseSucceed()
{
    bool firstflag = true;
    _new_hash_table.InitForNextElement();
    
    NextElemT * nextelem = NULL;
    while ((nextelem = _new_hash_table.NextElement()) != NULL)
    {
        MemInfoT * curr = (MemInfoT *)nextelem;
    	//printf("[MemoryCheck]PrintReleaseSucceed nextelem:0x%08x, curr:0x%08x, curr->_state:%d\n", nextelem->_real_key, curr->_next._real_key, curr->_state);
        if (curr->_state == MEM_STATE_RELEASED)
        {
            if (firstflag)
            {
                WriteToFile(CORRECT_WRITE, "------------------------------------------------------------------- \n");
                WriteToFile(CORRECT_WRITE, "[MemoryCheck][RELEASE_SUCCEED]Release memory succeed! as blow: \n");
                firstflag = false;
            }

            WriteToFile(CORRECT_WRITE, "[MemoryCheck][RELEASE_SUCCEED]Memory addr:0x%08x, size:%u, release type:%s, [file:%s, func:%s, line:%u].\n",
                curr->_next._real_key, curr->_size, OP_NAME[curr->_type], curr->_file, curr->_func, curr->_line);
        }
    }
}

/*************************************************************************************
* 函数名称: MemoryCheck::PrintBacktrace
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  打印函数调用栈
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::PrintBacktrace(const char * format, ...)
{
	if (_incorrect_released_bt == NULL)
	{
		_incorrect_released_bt = fopen("./result/memory_check_bt.log", "w");
		if (_incorrect_released_bt == NULL)
		{
			return;
		}
	}

	char buff[1024];
	memset(buff, 0, sizeof(buff));
	va_list args;
    va_start(args, format);
	vsnprintf(buff, sizeof(buff), format, args);
	va_end(args);
	
	fwrite(buff, strlen(buff), 1, _incorrect_released_bt);

#ifdef __linux__
    void *buffer[20];
    memset(buffer, 0, sizeof(buffer));
    size_t size = backtrace(buffer, 20);
    char **strings = backtrace_symbols(buffer, size);
    if (strings != NULL)
    {
        snprintf(buff, sizeof(buff), "Obtained %zd stack frames.\n", size);
        fwrite(buff, strlen(buff), 1, _incorrect_released_bt);
        for (size_t i = 0; i < size; i++)
        {
            snprintf(buff, sizeof(buff), "%s.\n", strings[i]);
        	fwrite(buff, strlen(buff), 1, _incorrect_released_bt);
        }

		fflush(_incorrect_released_bt);
        free(strings);
        strings = NULL;
    }
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

/*************************************************************************************
* 函数名称: MemoryCheck::GetBacktrace
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能:  获取函数调用栈信息
* 返 回 值: void
*************************************************************************************/
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

/*************************************************************************************
* 函数名称: MemoryCheck::WriteToFile
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能: 写文件
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::WriteToFile(int type, const char * format, ...)
{
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
	if (type == CORRECT_WRITE)
	{
		DoWriteToFile(_correct_released_file, "./result/memory_check_correct.log", "a", buffer, strlen(buffer));
	}
	else if (type == INCORRECT_WRITE)
	{
		DoWriteToFile(_incorrect_released_file, "./result/memory_check_incorrect.log", "w", buffer, strlen(buffer));
	}	
}

/*************************************************************************************
* 函数名称: MemoryCheck::DoWriteToFile
* 作     者:  lijun
* 日     期:  2019.03.29
* 参     数:
* 功     能: 写文件
* 返 回 值: void
*************************************************************************************/
void MemoryCheck::DoWriteToFile(FILE *& file, const char * filemame, const char * mode, char * data, size_t len)
{
	if (file == NULL)
	{
		file = fopen(filemame, mode);
		if (file == NULL)
		{
			printf("[MemoryCheck][ERROR]Open file() fail!\n", filemame);
			return;
		}

		char timebuf[32];
		GetNowTime(timebuf, sizeof(timebuf));

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer, sizeof(buffer), "Begin(%s)------------------------------------------------------------------- \n", timebuf);
		fwrite(buffer, strlen(buffer), 1, file);
	}

	fwrite(data, len, 1, file);
	fflush(file);
}

