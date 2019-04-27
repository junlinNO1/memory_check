#include "__hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

const unsigned int SEED = 2654435769u;

/*************************************************************************************
* 函数名称: hash_high32bit_to_l1_index
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? unsigned long - 待转换hash值的数据
* �?    �? 将unsigned long的高32bit转换为hashtable的第一级索�?* �?�?�? unsigned int - 索引�?*************************************************************************************/
unsigned int hash_high32bit_to_l1_index(unsigned long value)
{
    unsigned int high_32bits = (value & 0xFFFFFFFF00000000) >> 32;
    return ((high_32bits * SEED) >> 26) & 0x000003FF;
}

/*************************************************************************************
* 函数名称: hash_low32bit_to_l2_index
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? unsigned long - 待转换hash值的数据
* �?    �? 将unsigned long的低32bit转换为hashtable的第二级索引
* �?�?�? unsigned int - 索引�?*************************************************************************************/
unsigned int hash_low32bit_to_l2_index(unsigned long value)
{
    unsigned int low_32bits = (value & 0x00000000FFFFFFFF);
    return ((low_32bits * SEED) >> 20) & 0x0000FFFF;
}

/*************************************************************************************
* 函数名称: _HashTable::_HashTable
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? level1_table_size  - hashtable第一级表的容�?            level2_table_size - hashtable第二级表的容�?* �?    �?  hashtable 构造函�?* �?�?�? void
*************************************************************************************/
_HashTable::_HashTable(unsigned int level1_table_size/* = 0x40*/, unsigned int level2_table_size/* = 0x1000*/)
    : _level1_table_size(level1_table_size)
    , _level2_table_size(level2_table_size)
    , _table(NULL)
    , _current(NULL)
    , _release_elem(NULL)
{
    _table = (Level1Node*)malloc(_level1_table_size * sizeof(Level1Node));
    for (unsigned int idx = 0; idx < _level1_table_size; ++idx)
    {
        _table[idx]._level2 = (Level2Node*)malloc(_level2_table_size * sizeof(Level2Node));
        memset(_table[idx]._level2, 0, _level2_table_size * sizeof(Level2Node));
    }
}

/*************************************************************************************
* 函数名称: _HashTable::~_HashTable
* �?    �?	lijun 277974287@qq.com
* �?    �?	2019.04.24
* �?    �?  NONE
* �?    �?	hashtable 析构函数
* �?�?�? NONE
*************************************************************************************/
_HashTable::~_HashTable()
{
    if (_table != NULL)
    {
        for (unsigned int idx = 0; idx < _level1_table_size; ++idx)
        {
            if (_table[idx]._level2 != NULL)
            {
                free(_table[idx]._level2);
                _table[idx]._level2 = NULL;
            }
        }
        free(_table);
        _table = NULL;
    }
}

/*************************************************************************************
* 函数名称: _HashTable::RegesiterReleaseFunc
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? ReleaseElemFunc - 回调函数；重置hashtable时，如果存储的元素需要释放，此参数用于传递释放函�?* �?    �? 注册hashtable内存存储元素的释放接�?* �?�?�? void
*************************************************************************************/
void _HashTable::RegesiterReleaseFunc(ReleaseElemFunc release_func)
{
	if (release_func != NULL)
	{
		_release_elem = release_func;
	}
}

/*************************************************************************************
* 函数名称: _HashTable::ResetTable
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? ReleaseElemFunc - 回调函数；重置hashtable时，如果存储的元素需要释放，此参数用于传递释放函�?* �?    �? 重置hashtable 表中的数�?* �?�?�? void
*************************************************************************************/
void _HashTable::ResetTable(ReleaseElemFunc release_func/* = NULL */)
{
    if (_table != NULL)
    {
        for (unsigned int idx_l1 = 0; idx_l1 < _level1_table_size; ++idx_l1)
        {
            if (_table[idx_l1]._level2 == NULL)
            {
                continue;
            }
            
            for (unsigned idx_l2 = 0; idx_l2 < _level2_table_size; ++idx_l2)
            {
                NextElemT * next = NULL;
                NextElemT * del = _table[idx_l1]._level2[idx_l2]._info;
                while (del != NULL)
                {
                    next = del->_next;
                    if (release_func != NULL)
                    {
                    	release_func(del);
                    }
                    else if (_release_elem != NULL)
                    {
                    	_release_elem(del);
                    }
                    del = next;
                }
                _table[idx_l1]._level2[idx_l2]._info = NULL;
                _table[idx_l1]._level2[idx_l2]._count = 0;
            }
        }
    }

    _level1_cur_idx = 0;
    _level2_cur_idx = 0;
    _current = NULL;
}

/*************************************************************************************
* 函数名称: _HashTable::InsertElement
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? NextElemT * - 待插入的元素
* �?    �? 向hashtable表中插入元素
* �?�?�? int : 0 - 成功�?-1 - 失败
*************************************************************************************/
int _HashTable::InsertElement(unsigned long key, NextElemT * elem)
{
	if (elem == NULL) return -1;

    unsigned int level1_idx = hash_high32bit_to_l1_index(key);
    unsigned int level2_idx = hash_low32bit_to_l2_index(key);

    _hashlock.lock();
    if (_table[level1_idx]._level2[level2_idx]._info == NULL)
    {
        _table[level1_idx]._level2[level2_idx]._info = elem;
        ++_table[level1_idx]._count;
    }
    else
    {
        elem->_next = _table[level1_idx]._level2[level2_idx]._info;
        _table[level1_idx]._level2[level2_idx]._info = elem;
    }
    ++_table[level1_idx]._level2[level2_idx]._count;
    _hashlock.unlock();

    return 0;
}

/*************************************************************************************
* 函数名称: _HashTable::FindElement
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? unsigned long - 待查找的键�?	        NextElemT *&   - 输出参数，找到的元素，或NULL
* �?    �? 在表中根据传入的key查找存储元素
* �?�?�? int : 0 - 成功�?-1 - 失败
*************************************************************************************/
int _HashTable::FindElement(unsigned long key, NextElemT *& elem)
{
    unsigned int level1_idx = hash_high32bit_to_l1_index(key);
    unsigned int level2_idx = hash_low32bit_to_l2_index(key);

    elem = NULL;
    _hashlock.lock();
    NextElemT * cur = _table[level1_idx]._level2[level2_idx]._info;
    _hashlock.unlock();

    while (cur != NULL && cur->_real_key != key)
    {
        cur = cur->_next;
    }

    if (cur != NULL)
    {
        elem = cur;
        return 0;
    }

    return -1;
}

/*************************************************************************************
* 函数名称: _HashTable::FindAndDeleteElement
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? unsigned long    - 待查找的键�?			ReleaseElemFunc - 释放元素的回调函数，默认值NULL
* �?    �? 在表中根据传入的key查找存储元素, 若找到则从表中删除该元素
* �?�?�? int : 0 - 成功�?-1 - 失败
*************************************************************************************/
int _HashTable::FindAndDeleteElement(unsigned long key, ReleaseElemFunc release_func/*=NULL*/)
{
    unsigned int level1_idx = hash_high32bit_to_l1_index(key);
    unsigned int level2_idx = hash_low32bit_to_l2_index(key);

    _hashlock.lock();
    NextElemT * cur = _table[level1_idx]._level2[level2_idx]._info;
    NextElemT * last = NULL;
    while (cur != NULL && cur->_real_key != key)
    {
        last = cur;
        cur = cur->_next;
    }

    if (cur != NULL)
    {
        if (cur == _table[level1_idx]._level2[level2_idx]._info && last == NULL)
        {
            _table[level1_idx]._level2[level2_idx]._info = cur->_next;
        }
        else
        {
            last->_next = cur->_next;
        }

        _hashlock.unlock();

		if (release_func != NULL) {
			release_func(cur);
		}
		else if (_release_elem != NULL) {
        	_release_elem(cur);
        }
        return 0;
    }

	_hashlock.unlock();
    return -1;
}

/*************************************************************************************
* 函数名称: _HashTable::InitForNextElement
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? void
* �?    �? 为了实现遍历表中的所有元素，首次遍历前需要初始化，遍历过程中不能再调用，否则重新遍历
* �?�?�? void
*************************************************************************************/
void _HashTable::InitForNextElement()
{
    _level1_cur_idx = 0;
    _level2_cur_idx = 0;
    _current = NULL;
}

/*************************************************************************************
* 函数名称: _HashTable::NextElement
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? void
* �?    �? 遍历表中的所有元素，每调用一次，返回一个指向该元素的指�?* �?�?�? NextElemT *  - 指向元素的指�?*************************************************************************************/
NextElemT * _HashTable::NextElement()
{
    if (_table != NULL)
    {
        while (_level1_cur_idx < _level1_table_size)
        {
            if (_table[_level1_cur_idx]._level2 == NULL)
            {
                ++_level1_cur_idx;
                _level2_cur_idx = 0;
                continue;
            }

            if (_current == NULL)
            {
                while (_level2_cur_idx < _level2_table_size)
                {
                    _current = _table[_level1_cur_idx]._level2[_level2_cur_idx++]._info;
                    if (_current != NULL)
                    {
                        NextElemT * tmp = _current;
                        _current = _current->_next;
                        return tmp;
                    }
                }
                
                ++_level1_cur_idx;
                _level2_cur_idx = 0;
            }
            else
            {
                NextElemT * tmp = _current;
                _current = _current->_next;
                return tmp;
            }
        }
    }
    return NULL;
}

/*************************************************************************************
* 函数名称: _HashTable::PrintHashScoredHit
* �?    �?  lijun 277974287@qq.com
* �?    �?  2019.04.24
* �?    �? void
* �?    �? 打印hashtable的命中情�?* �?�?�? void
*************************************************************************************/
void _HashTable::PrintHashScoredHit()
{
	if (_table != NULL)
    {
    	FILE * file = fopen("./hash_scored_hit.log", "a");
		if (file == NULL)
		{
			printf("[_HashTable][ERROR]PrintHashScoredHit create file(./hash_scored_hit.log) fail!\n");
			return;
		}

		char buffer[1024];
        for (unsigned int idx_l1 = 0; idx_l1 < _level1_table_size; ++idx_l1)
        {
        	memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "Level 1 (index: %d, hit count: %d)------\n", idx_l1, _table[idx_l1]._count);
            fwrite(buffer, strlen(buffer), 1, file);
            for (unsigned idx_l2 = 0; idx_l2 < _level2_table_size; ++idx_l2)
            {
            	if (_table[idx_l1]._level2 == NULL) {
            		continue;
            	}
            	
                memset(buffer, 0, sizeof(buffer));
	            sprintf(buffer, "--Level 2 (index: %d, hit count: %d)\n", idx_l2, _table[idx_l1]._level2[idx_l2]._count);
	            fwrite(buffer, strlen(buffer), 1, file);
            }
        }
        fflush(file);
        fclose(file);
    }
}

