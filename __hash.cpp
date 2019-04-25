#include "__hash.h"
#include <iostream>

const unsigned int SEED = 2654435769;

unsigned int hash_high32bit_to_10bit(unsigned long value)
{
    unsigned int high_32bits = (value & 0xFFFFFFFF00000000) >> 32;
    return ((high_32bits * SEED) >> 26) & 0x000003FF;
}

unsigned int hash_low32bit_to_16bit(unsigned long value)
{
    unsigned int low_32bits = (value & 0x00000000FFFFFFFF);
    return ((low_32bits * SEED) >> 20) & 0x0000FFFF;
}

_HashTable::_HashTable(unsigned int level1_table_size/* = 0x100*/, unsigned int level2_table_size/* = 0x400*/)
    : _level1_table_size(level1_table_size)
    , _level2_table_size(level2_table_size)
    , _level1_cur_num(0)
    , _level2_cur_num(0)
    , _hash_table(NULL)
{
    _hash_table = (Level1Node*)malloc(_level1_table_size * sizeof(Level1Node));
    for (unsigned int idx = 0; idx < _level1_table_size; ++idx)
    {
        _hash_table[idx]._level2 = (Level2Node*)malloc(_level2_table_size * sizeof(Level2Node));
        memset(_hash_table[idx]._level2, 0, _level2_table_size * sizeof(Level2Node));
    }
}

_HashTable::~_HashTable()
{
    if (_hash_table != NULL)
    {
        for (unsigned int idx = 0; idx < _level1_table_size; ++idx)
        {
            if (_hash_table[idx]._level2 != NULL)
            {
                free(_hash_table[idx]._level2);
                _hash_table[idx]._level2 = NULL;
            }
        }
        free(_hash_table);
        _hash_table = NULL;
    }
}

void _HashTable::CreateHashTable()
{
    if (_hash_table == NULL) 
    {
        _hash_table = (Level1Node*)malloc(_level1_table_size * sizeof(Level1Node));
        for (unsigned int idx = 0; idx < _level1_table_size; ++idx)
        {
            _hash_table[idx]._level2 = (Level2Node*)malloc(_level2_table_size * sizeof(Level2Node));
            memset(_hash_table[idx]._level2, 0, _level2_table_size * sizeof(Level2Node));
        }
    }
}

void _HashTable::Reset()
{
    if (_hash_table != NULL)
    {
        for (unsigned int idx_l1 = 0; idx_l1 < _level1_table_size; ++idx_l1)
        {
            if (_hash_table[idx_l1]._level2 == NULL)
            {
                continue;
            }
            
            for (unsigned idx_l2 = 0; idx_l2 < _level2_table_size; ++idx_l2)
            {
                MemInfoT * next = NULL;
                MemInfoT * del = _hash_table[idx_l1]._level2[idx_l2]._info;
                while (del != NULL)
                {
                    next = del->_next;
                    free(del);
                    del = next;
                }
                _hash_table[idx_l1]._level2[idx_l2]._info = NULL;
                _hash_table[idx_l1]._level2[idx_l2]._count = 0;
            }
        }
    }

    _level1_cur_idx = 0;
    _level2_cur_idx = 0;
    _cur = NULL;
}

int _HashTable::InsertElement(MemInfoT * meminfo)
{
    MemInfoT * newmeminfo = (MemInfoT *)malloc(sizeof(MemInfoT));
    if (meminfo == NULL)
    {
        printf("[MemoryCheck][Malloc_Error]_HashTable::InsertElement, malloc meminfo fail!.\n");
        return -1;
    }

    *newmeminfo = *meminfo;

    unsigned long value = reinterpret_cast<unsigned long>(newmeminfo->_ptr);
    unsigned int level1_idx = hash_high32bit_to_10bit(value);
    unsigned int level2_idx = hash_low32bit_to_16bit(value);

    _hashlock.lock();
    if (_hash_table[level1_idx]._level2[level2_idx]._info == NULL)
    {
        _hash_table[level1_idx]._level2[level2_idx]._info = newmeminfo;
    }
    else
    {
        newmeminfo->_next = _hash_table[level1_idx]._level2[level2_idx]._info;
        _hash_table[level1_idx]._level2[level2_idx]._info = newmeminfo;
    }
    ++_hash_table[level1_idx]._level2[level2_idx]._count;
    _hashlock.unlock();

    return 0;
}

int _HashTable::FindElement(void * ptr, MemInfoT *& meminfo)
{
    unsigned long value = reinterpret_cast<unsigned long>(ptr);
    unsigned int level1_idx = hash_high32bit_to_10bit(value);
    unsigned int level2_idx = hash_low32bit_to_16bit(value);

    meminfo = NULL;
    if (_hash_table[level1_idx]._level2[level2_idx]._info == NULL)
    {
        return -1;
    }

    _hashlock.lock();
    MemInfoT * cur = _hash_table[level1_idx]._level2[level2_idx]._info;
    _hashlock.unlock();

    while (cur != NULL && cur->_ptr != ptr)
    {
        cur = cur->_next;
    }

    if (cur != NULL)
    {
        meminfo = cur;
        return 0;
    }

    return -1;
}

int _HashTable::FindAndDeleteElement(void * ptr)
{
    unsigned long value = reinterpret_cast<unsigned long>(ptr);
    unsigned int level1_idx = hash_high32bit_to_10bit(value);
    unsigned int level2_idx = hash_low32bit_to_16bit(value);

    if (_hash_table[level1_idx]._level2[level2_idx]._info == NULL)
    {
        return -1;
    }

    _hashlock.lock();
    MemInfoT * cur = _hash_table[level1_idx]._level2[level2_idx]._info;
    _hashlock.unlock();
    MemInfoT * last = NULL;
    while (cur != NULL && cur->_ptr != ptr)
    {
        last = cur;
        cur = cur->_next;
    }

    if (cur != NULL)
    {
        _hashlock.lock();
        if (cur == _hash_table[level1_idx]._level2[level2_idx]._info && last == NULL)
        {
            _hash_table[level1_idx]._level2[level2_idx]._info = cur->_next;
        }
        else
        {
            last->_next = cur->_next;
        }
        _hashlock.unlock();

        free(cur);
        return 0;
    }

    return -1;
}

MemInfoT * _HashTable::NextElement()
{
    if (_hash_table != NULL)
    {
        while (_level1_cur_idx < _level1_table_size)
        {
            if (_hash_table[_level1_cur_idx]._level2 == NULL)
            {
                ++_level1_cur_idx;
                _level2_cur_idx = 0;
                continue;
            }

            if (_cur == NULL)
            {
                while (_level2_cur_idx < _level2_table_size)
                {
                    _cur = _hash_table[_level1_cur_idx]._level2[_level2_cur_idx++]._info;
                    if (_cur != NULL)
                    {
                        MemInfoT * tmp = _cur;
                        _cur = tmp->_next;
                        return tmp;
                    }
                }
                
                ++_level1_cur_idx;
                _level2_cur_idx = 0;
            }
            else
            {
                MemInfoT * tmp = _cur;
                _cur = tmp->_next;
                return tmp;
            }
        }
    }
    return NULL;
}

void _HashTable::ResetNextElement()
{
    _level1_cur_idx = 0;
    _level2_cur_idx = 0;
    _cur = NULL;
}
