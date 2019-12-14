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
        Node<Type>* tail = tail_;
        do{
            while(tail->next != NULL)
                tail = tail->next;
        }while(!__sync_bool_compare_and_swap(&(p->next), NULL, node));
        __sync_bool_compare_and_swap(&(tail_), p, node);
    }

    bool pop(Type *res)
    {
        Node<Type>* front = head_->next;
        do{
            if(head_ == tail_)
                return false;
            front = head_->next;
        }while(!__sync_bool_compare_and_swap(&(head_->next), front, front->next));
        *res = front->val;
        delete front;
        return true;
    }

private:
    Node<Type>* head_;
    Node<Type>* tail_;
};