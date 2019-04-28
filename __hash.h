#ifndef __HASH_H_H__
#define __HASH_H_H__
#include "__autolock.h"

unsigned int hash_high32bit_to_l1_index(unsigned long long value);
unsigned int hash_low32bit_to_l2_index(unsigned long long value);

typedef struct _next_elem
{
	unsigned long _real_key;
	_next_elem * _next;

	_next_elem(unsigned long key = 0, _next_elem * next= NULL) : _real_key(key), _next(next) {}
}NextElemT;

typedef struct _level2_node
{
    unsigned int _count;
    NextElemT *  _info;

    _level2_node(unsigned int count = 0, NextElemT * info = NULL) : _count(count), _info(info) {}
}Level2Node;

typedef struct _level1_node
{
	unsigned int _count;
    Level2Node * _level2;

    _level1_node(unsigned int count = 0, Level2Node * level2 = NULL) : _count(count), _level2(level2) {}
}Level1Node;

typedef void (* ReleaseElemFunc)(NextElemT * elem);

class _HashTable
{
public:
    _HashTable(unsigned int level1_table_size = 0x40, unsigned int level2_table_size = 0x1000);
    ~_HashTable();

    int Initialize(unsigned int level1_table_size = 0x40, unsigned int level2_table_size = 0x1000);
	void RegesiterReleaseFunc(ReleaseElemFunc release_func);
    void ResetTable(ReleaseElemFunc release_func = NULL);
    int InsertElement(unsigned long key, NextElemT * elem);
    int FindElement(unsigned long key, NextElemT *& elem);
    int FindAndDeleteElement(unsigned long key, ReleaseElemFunc release_func = NULL);

    void InitForNextElement();
	NextElemT * NextElement();

	void PrintHashScoredHit();

private:
	unsigned int Level1Index(const unsigned long & key);
	unsigned int Level2Index(const unsigned long & key);

private:
    unsigned int _level1_table_size;
    unsigned int _level2_table_size;

    Level1Node * _table;
    ns_lock::_Lock _hashlock;

    //遍历hashtable
    unsigned int _level1_cur_idx;
    unsigned int _level2_cur_idx;
    NextElemT * _current;

    ReleaseElemFunc _release_elem;
};

#endif //__HASH_H_H__