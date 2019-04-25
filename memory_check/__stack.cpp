#include "__stack.h"

//��ӡElementT����
void PrintElement(const ElementT & elem)
{
    std::cout << elem._file << ", " << elem._func << ", " << elem._line << std::endl;
}

SimpleLinkStack::~SimpleLinkStack()
{
    ElementT elem;
    while (!isEmpty())
        Pop(elem);
    if (_headNode)
        free(_headNode);
}

//��������е�����Ԫ��
bool SimpleLinkStack::clear()
{
    while (_headNode->_next)
    {
        ElementT elem;
        if (!Pop(elem))
        {
            return false;
        }
    }
    return true;
}

//�ж�ջ�Ƿ�Ϊ��
bool SimpleLinkStack::isEmpty()
{
    if (!_length)
    {
        return true;
    }
    else
        return false;
}

//��ȡջ�ĳ���
unsigned int SimpleLinkStack::GetLength()
{
    return _length;
}

//��ȡջ��Ԫ�أ�ֻ���ʲ���ջ
const ElementT SimpleLinkStack::GetTop()
{
    if (_headNode->_next != NULL)
    {
        StackNode *top = _headNode->_next;
        return top->_elem;
    }
    return ElementT();
}

//��Ԫ������ջ��
bool SimpleLinkStack::Push(const ElementT elem)
{
    StackNode* pNode = (StackNode*)malloc(sizeof(StackNode));
    pNode->_elem = elem;

    //����ͷ�巨����pNode����ջ���ģ�����pNode����һ��ӦΪhead����һ����
    pNode->_next = _headNode->_next;
    _headNode->_next = pNode;
    _length++;
    return true;
}

//��ջ��Ԫ�س�ջ
bool SimpleLinkStack::Pop(ElementT & elem)
{
    if (_headNode->_next != NULL)
    {
        StackNode* top = _headNode->_next;
        _headNode->_next = top->_next;
        elem = top->_elem;
        _length--;
        free(top);
        return true;
    }
    else
        return false;
}

//��ӡ��ǰջ�е�����Ԫ��
void SimpleLinkStack::printStack()
{
    if (_headNode->_next != NULL)
    {
        StackNode* pNode = _headNode->_next;
        ElementT & elem = pNode->_elem;
        PrintElement(pNode->_elem);
        while (pNode->_next)
        {
            PrintElement(pNode->_next->_elem);
            pNode = pNode->_next;
        }
    }
}
