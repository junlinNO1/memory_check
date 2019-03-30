#include<iostream>
#include<map>
using namespace std;
//---------------------------------------------------------------
// �ڴ��¼
//---------------------------------------------------------------
class MemInfo {
private:
    void* ptr;
    const char* file;
    unsigned int line;
    const char* type;
    unsigned int size;
    MemInfo* link;
    friend class MemStack;
};
//---------------------------------------------------------------
// �ڴ��¼ջ 
//---------------------------------------------------------------
class MemStack {
private:
    MemInfo* head;
public:
    MemStack() :head(NULL) { }
    ~MemStack() {
        MemInfo* tmp;
        while (head != NULL) {
            free(head->ptr); // �ͷ�й©���ڴ� 
            tmp = head->link;
            free(head);
            head = tmp;
        }
    }
    void Insert(void* ptr, const char* file, unsigned int line, const char * type, unsigned int size) {
        MemInfo* node = (MemInfo*)malloc(sizeof(MemInfo));
        node->ptr = ptr; node->file = file; node->line = line; node->type = type; node->size = size;
        node->link = head; head = node;
    }
    void Delete(void* ptr) {
        MemInfo* node = head;
        MemInfo* pre = NULL;
        while (node != NULL && node->ptr != ptr) {
            pre = node;
            node = node->link;
        }
        if (node == NULL)
            cout << "ɾ��һ��û�п��ٵ��ڴ�" << endl;
        else {
            if (pre == NULL) // ɾ������head 
                head = node->link;
            else
                pre->link = node->link;
            free(node);
        }
    }
    void Print() {
        if (head == NULL) {
            cout << "�ڴ涼�ͷŵ���" << endl;
            return;
        }
        cout << "���ڴ�й¶����" << endl;
        MemInfo* node = head;
        while (node != NULL) {
            cout << "�ļ���: " << node->file << " , " << "����: " << node->line << " , "
                << "���ͣ�" << node->type << " , "
                << "��ַ: 0x" << node->ptr << " , ��С(byte)��" << node->size << endl;
            node = node->link;
        }
        cout << __FUNCTION__ << endl;
    }
};
//---------------------------------------------------------------
// ȫ�ֶ��� mem_stack��¼���ٵ��ڴ� 
//---------------------------------------------------------------
MemStack mem_stack;
//---------------------------------------------------------------
// ����new,new[],delete,delete[] 
//---------------------------------------------------------------
void* operator new(size_t size, const char* file, unsigned int line){
    void* ptr = malloc(size);
    mem_stack.Insert(ptr, file, line, "delete", size);
    return ptr;
}
void* operator new[](size_t size, const char* file, unsigned int line) {
    //return operator new(size, file, line); // ������new 
    void* ptr = malloc(size);
    mem_stack.Insert(ptr, file, line, "delete[]", size);
    return ptr;
}
void operator delete(void* ptr) {
    free(ptr);
    mem_stack.Delete(ptr);
}
void operator delete[](void* ptr) {
    operator delete(ptr);
}
//---------------------------------------------------------------
// ʹ�ú꽫�����Դ����е�new��delte�滻Ϊ���ص�new��delete 
//---------------------------------------------------------------
#define new new(__FILE__,__LINE__)
//---------------------------------------------------------------
// �����Դ��� 
//---------------------------------------------------------------
void bad_code() {
    int *p = new int;
    char *q = new char[5];
    delete[]q;
}

void good_code() {
    int *p = new int;
    char *q = new char[5];
    delete p;
    delete[]q;
}
//---------------------------------------------------------------
// ���Թ��� 
//---------------------------------------------------------------

extern void testfunc();
extern void testfunc2();
int main1() {
    good_code();
    bad_code();

    testfunc();

    testfunc2();

    mem_stack.Print();
    system("PAUSE");
    return 0;
}