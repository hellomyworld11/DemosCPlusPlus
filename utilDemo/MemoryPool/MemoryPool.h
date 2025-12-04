#pragma once

template<typename T, size_t B = 4096>
class MemoryPool {
public:
	//type
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	//用来重新绑定，把原本类型T 转成新类型U ，为了兼容各种自定义类型的内存管理
	template<typename U> struct rebind {
		typedef MemoryPool<U> other;
	};
	//throw()表示不抛出异常
	MemoryPool()throw();
	MemoryPool(const MemoryPool& memoryPool)throw();
	//复制拷贝构造的模板：支持不同类型来构造
	template<typename U> MemoryPool(const MemoryPool<U>& memoryPool)throw();

	~MemoryPool() throw();

	//申请内存
	pointer allocate(size_type n = 1, const_pointer hint = 0);
	//释放内存
	void deallocate(pointer p, size_type n = 1);

	//取址
private:
	union Slot_
	{
		value_type element;
		Slot_* next;
	};

	typedef char* data_pointer_;
	typedef Slot_ slot_type_;
	typedef Slot_* slot_pointer_;

	//申请大块内存
	void allocateBlock();
	//把p对齐到align的偏移量
	size_type padPointer(data_pointer_ p, size_type align)const throw();

	slot_pointer_ currentBlock_;  //实际上的内存首地址
	slot_pointer_ currentSlot_;  //应用层代表没有被申请的内存首地址
	slot_pointer_ lastSlot_;    //应用层代表没有被申请的内存最后首地址
	slot_pointer_ freeSlots_;  //元素释放后，没存放元素的空闲空间 

};

