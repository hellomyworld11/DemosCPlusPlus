#ifndef MEMORYPOOL_TCC
#define MEMORYPOOL_TCC

template<typename T, size_t B>
MemoryPool<T, B>::MemoryPool()
throw()
{
	currentBlock_ = nullptr;
	currentSlot_ = nullptr;
	lastSlot_ = nullptr;
	freeSlots_ = nullptr;
}

template <typename T, size_t B>
template<typename U>
MemoryPool<T, B>::MemoryPool(const MemoryPool<U>& memoryPool) throw()
{
	MemoryPool();
}

template<typename T, size_t B /*= 4096*/>
MemoryPool<T, B>::MemoryPool(const MemoryPool& memoryPool) throw()
{
	MemoryPool();
}

template<typename T, size_t B /*= 4096*/>
MemoryPool<T, B>::~MemoryPool() 
throw()
{
	slot_pointer_ cur = currentBlock_;
	while (cur != nullptr)
	{
		slot_pointer_ nxt = cur->next;
		operator delete(reinterpret_cast<void*>(cur));
		cur = nxt;
	}
}

template<typename T, size_t B>
inline typename MemoryPool<T, B>::size_type MemoryPool<T, B>::padPointer(data_pointer_ p, size_type align) const throw()
{
	size_type addr = reinterpret_cast<size_type>(p);
	size_type rem = addr % align;
	return (rem == 0) ? 0 : (align - rem);
}


template<typename T, size_t B>
void MemoryPool<T, B>::allocateBlock()
{
	//申请分配一块新内存   operator new 函数只分配内存，不调用构造函数
	data_pointer_ newBlock = reinterpret_cast<data_pointer_>(operator new(B));
	//头插法
	reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
	currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
	//对齐， 首先必须满足存储指针的最小空间。
	data_pointer_ body = newBlock + sizeof(slot_pointer_);
	//按 自定义类型对齐。
	size_type bodyPadding = padPointer(body, sizeof(slot_type_));
	//偏移指针到对齐后的地址。
	currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
	//最后能放置slot_type_的位置
	lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + B - sizeof(slot_type_) + 1);
}

template<typename T, size_t B>
inline typename MemoryPool<T, B>::pointer
MemoryPool<T, B>::allocate(size_type n /* = 1 */, const_pointer hint /* = 0 */)
{
	if (freeSlots_ != nullptr)
	{
		//当需要存储元素时，把slot指针转成pointer
		pointer result = reinterpret_cast<pointer>(freeSlots_);
		freeSlots_ = freeSlots_->next;
		return result;
	}
	else {
		if (currentSlot_ >= lastSlot_)
			allocateBlock();

		return reinterpret_cast<pointer>(currentSlot_++);
	}
}

template<typename T, size_t B /*= 4096*/>
inline void MemoryPool<T, B>::deallocate(pointer p, size_type n /*= 1*/)
{
	if (p != nullptr)
	{
		reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
		freeSlots_ = reinterpret_cast<slot_pointer_>(p);
	}
}

template<typename T, size_t B /*= 4096*/>
inline void MemoryPool<T, B>::construct(pointer p, const_reference val)
{
	new (p) value_type(val);
}

template<typename T, size_t B /*= 4096*/>
inline void MemoryPool<T, B>::destroy(pointer p)
{
	p->~value_type();
}

template<typename T, size_t B /*= 4096*/>
inline typename MemoryPool<T, B>::pointer MemoryPool<T, B>::newElement(const_reference val)
{
	//1.分配空间
	pointer pNewElem = allocate();
	//2.调用构造函数
	construct(pNewElem, val);
	return pNewElem;
}

template<typename T, size_t B /*= 4096*/>
inline void MemoryPool<T, B>::deleteElement(pointer p)
{
	destroy(p);
	deallocate(p);
}

template<typename T, size_t B /*= 4096*/>
inline typename MemoryPool<T, B>::pointer MemoryPool<T, B>::address(reference x) const throw()
{
	return &x;
}

template<typename T, size_t B /*= 4096*/>
inline typename MemoryPool<T, B>::const_pointer MemoryPool<T, B>::address(const_reference x) const throw()
{
	return &x;
}

template<typename T, size_t B /*= 4096*/>
inline typename MemoryPool<T, B>::size_type MemoryPool<T, B>::max_size() const throw()
{
	//-1的无符号二进制是 FFFFFFFF
	size_type maxBlocks = -1 / B;  
	return (B - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}


#endif