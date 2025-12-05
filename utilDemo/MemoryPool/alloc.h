#pragma once
#include <memory>

template <typename T>
struct LinkNode {
	T data;
	LinkNode *next = nullptr;
};

template <typename T, class Alloc = std::allocator<T>>
class SimpleAlloc {
public:
	typedef LinkNode<T> Node;
	//将分配器的模板类型从T改成 Node<T>
	//struct rebind  属于std::allocator类，用来重新绑定为新类型
	//{
	//	typedef allocator<U> other;
	//};
	typedef typename Alloc::template rebind<Node>::other NodeAlloctor;

	SimpleAlloc() { head_ = nullptr; }

	~SimpleAlloc() { clear(); }

	bool empty() {
		return head_ == nullptr;
	}

	void clear()
	{
		while (head_!=nullptr)
		{
			Node *next = head_->next;
			alloc_.destroy(head_);
			alloc_.deallocate(head_, 1);
			head_ = next;
		}
	}

	void push(T elem)
	{
		Node *newnode = alloc_.allocate(1);  // new 运算符 底层先分配内存，再调用构造函数
		alloc_.construct(newnode, Node());

		newnode->data = elem;
		newnode->next = head_;
		head_ = newnode;
	}

	T pop()
	{
		T ret = head_->data;
		Node *newhead = head_->next;
		alloc_.destroy(head_);
		alloc_.deallocate(head_, 1);
		head_ = newhead;
		return ret;
	}

	T top()
	{
		return head_->data;
	}
private:
	
	Node *head_;

	NodeAlloctor alloc_;
};
