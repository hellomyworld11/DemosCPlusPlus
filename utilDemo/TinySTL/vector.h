#pragma once

#include "iterator.h"
#include "memory.h"

namespace TSL 
{


template<class T>
class vector 
{
public:
	typedef TSL::allocator<T>	allocator_type;
	typedef TSL::allocator<T>	data_allocator;

	typedef typename allocator_type::value_type      value_type;
	typedef typename allocator_type::pointer         pointer;
	typedef typename allocator_type::const_pointer   const_pointer;
	typedef typename allocator_type::reference       reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::size_type       size_type;
	typedef typename allocator_type::difference_type difference_type;

	typedef value_type*  iterator;
	typedef const value_type* const_iterator;
	typedef TSL::reverse_iterator<iterator> reverse_iterator;
	typedef TSL::reverse_iterator<const_iterator> const_reverse_iterator;

	allocator_type get_allocator() { return data_allocator(); }
private:
	iterator begin_;		//使用空间头部
	iterator end_;		//使用空间尾部
	iterator cap_;    //存储空间尾部
public:
	vector() noexcept { try_init();}
	
	explicit vector(size_type n)
	{

	}



private:
	void try_init()noexcept;

	void fill_init(size_type n, const value_type& value);

	void init_space(size_type size, size_type cap);
};

template<class T>
void TSL::vector<T>::init_space(size_type size, size_type cap)
{
	try
	{
		begin_ = data_allocator::allocate(cap);
		end_ = begin_ + size;
		cap_ = begin_ + cap;
	}
	catch (...)
	{
		begin_ = nullptr;
		end_ = nullptr;
		cap_ = nullptr;
		throw();
	}
}

template<class T>
void vector<T>::try_init() noexcept
{
	try
	{
		begin_ = data_allocator::allocate(16);
		end_ = begin_;
		cap_ = begin_ + 16;
	}
	catch (...)
	{
		begin_ = nullptr;
		end_ = nullptr;
		cap_ = nullptr;
	}
}

template<class T>
void TSL::vector<T>::fill_init(size_type n, const value_type& value)
{
	const size_type init_size = TSL::max(n, static_cast<size_type>(16));
	init_space(n, init_size);

}



}
