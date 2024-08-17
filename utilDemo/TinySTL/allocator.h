#pragma once


//ÅäÖÃÆ÷ £¿
namespace stl
{
	template<class T>
	class allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef size_t size_type;
	public:
		static T* allocate();
		static T* allocate(size_type n);

		static void deallocate(T* ptr);
		static void deallocate(T* ptr, size_type n);

		static void construct(T* ptr);
		static void construct(T* ptr, const T& value);
		static void construct(T* ptr, T&& value); //?

		//?
		template <class... Args>
		static void construct(T* ptr, Args&& ...args);

		static void destroy(T* ptr);
		static void destroy(T* first, T* last);
	};

	template<class T>
	void stl::allocator<T>::construct(T* ptr)
	{

	}

	template<class T>
	T* allocator<T>::allocate()
	{   //operator new ½ö·ÖÅäÄÚ´æ 
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
	void stl::allocator<T>::deallocate(T* ptr, size_type n)
	{
		if (ptr != nullptr)
			::operator delete(ptr);
	}













}

