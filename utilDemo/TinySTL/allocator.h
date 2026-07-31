#pragma once

#include "construct.h"


//内存分配器  
namespace TSL
{
	template<class T>
	class allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
	public:
		//只分配内存
		static T* allocate();
		static T* allocate(size_type n);
		//释放内存
		static void deallocate(T* ptr);
		static void deallocate(T* ptr, size_type n);
		//调用构造
		static void construct(T* ptr);
		static void construct(T* ptr, const T& value);
		static void construct(T* ptr, T&& value); //?

		//?
		template <class... Args>
		static void construct(T* ptr, Args&& ...args);
		//调用析构
		static void destroy(T* ptr);
		static void destroy(T* first, T* last);
	};

	template<class T>
	void TSL::allocator<T>::destroy(T* first, T* last)
	{
		TSL::destory(first, last);
	}

	template<class T>
	void TSL::allocator<T>::destroy(T* ptr)
	{
		TSL::destory(ptr);
	}

	template <class::::::Args>
	void TSL::allocator<>::construct(T* ptr, Args&& ...args)
	{
		TSL::construct(ptr, std::forward<Args>(args)...);
	}

	template<class T>
	void TSL::allocator<T>::construct(T* ptr, T&& value)
	{
		TSL::construct(ptr, std::move(value));
	}

	template<class T>
	void TSL::allocator<T>::construct(T* ptr, const T& value)
	{
		TSL::construct(ptr, value);
	}

	template<class T>
	void allocator<T>::construct(T* ptr)
	{
		TSL::construct(ptr);
	}

	template<class T>
	T* allocator<T>::allocate()
	{   //operator new 仅分配内存 
		return static_cast<T*>(::operator new(sizeof(T)));
	}

	template<class T>
	T* allocator<T>::allocate(size_type n)
	{
		if (0 >= n) return nullptr;
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	template<class T>
	void allocator<T>::deallocate(T* ptr)
	{
		if (ptr != nullptr)
			::operator delete(ptr);
	}

	template<class T>
	void allocator<T>::deallocate(T* ptr, size_type n)
	{
		if (ptr != nullptr)
			::operator delete(ptr);
	}













}

