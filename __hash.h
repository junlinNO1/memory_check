#ifndef __HASH_H_H__
#define __HASH_H_H__
#include "__autolock.h"
#include "__memory_info.h"

unsigned int hash_high32bit_to_16bit(unsigned long long value);
unsigned int hash_low32bit_to_16bit(unsigned long long value);

typedef struct _level2_node
{
    unsigned int _count;
    MemInfoT *   _info;

    _level2_node() : _count(0), _info(NULL) {}
}Level2Node;

typedef struct _level1_node
{
    Level2Node * _level2;

    _level1_node() : _level2(NULL) {}
}Level1Node;

class _HashTable
{
public:
    _HashTable(unsigned int level1_table_size = 0x40, unsigned int level2_table_size = 0x1000);
    ~_HashTable();

    void CreateHashTable();

    int InsertElement(MemInfoT * meminfo);
    int FindElement(void * ptr, MemInfoT *& meminfo);
    int FindAndDeleteElement(void * ptr);

    void Reset();

    MemInfoT * NextElement();
    void ResetNextElement();

private:
    unsigned int _level1_table_size;
    unsigned int _level2_table_size;
    unsigned int _level1_cur_num;
    unsigned int _level2_cur_num;

    Level1Node * _hash_table;
    ns_lock::_Lock _hashlock;

    //”√”⁄±È¿˙hashtable
    unsigned int _level1_cur_idx;
    unsigned int _level2_cur_idx;
    MemInfoT * _cur;
};

#endif //__HASH_H_H__