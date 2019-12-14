#include <atomic>
#include <unistd.h>

template<typename Type>
class Node
{
public:
    Node(): next(NULL){}
    Node(Type v): val(v), next(NULL){}
    Node* next;
    Type  val;
};

template<typename Type>
class LockFreeQueue
{
public:
    LockFreeQueue():
        head_(new Node<Type>()),
        tail_(head_)
    {}

    void push(Type val)
    {
        Node<Type>* node = new Node<Type>(val);
        Node<Type>* p = tail_;
        do{
            while(p->next != NULL)
                p = p->next;
        }while(!__sync_bool_compare_and_swap(p->next, NULL, node));
        __sync_bool_compare_and_swap(tail_, p, node);
    }

    bool pop(Type *res)
    {
        Node<Type>* node = head_->next;
        do{
            node = head_->next;
            if(head_ == tail_)
                return false;
        }(!__sync_bool_compare_and_swap(head_->next, node, node->next));
        *res = node->val;
        delete node;
        return true;
    }

private:
    Node<Type>* head_;
    Node<Type>* tail_;
};