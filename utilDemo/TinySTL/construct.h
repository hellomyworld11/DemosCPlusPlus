#pragma once

#include "type_traits.h"
#include "iterator.h"
namespace TSL {

//构造和析构

template<class Ty>
void construct(Ty* ptr)
{
	::new ((void*)ptr) Ty();
}

template<class Ty, class Ty2>
void construct(Ty* ptr, const Ty2& value)
{
	::new ((void*)ptr) Ty(value);
}

template<class Ty, class... Args>
void construct(Ty* ptr, Args&&... args)
{
	::new ((void*)ptr) Ty(TSL::forward<Args>(args)...);
}

//如果普通类型，不需要调用析构函数
template<class Ty>
void destroy_one(Ty*, std::true_type) {}

//如果自定义类型，需要调用析构函数
template<class Ty>
void destroy_one(Ty* ptr, std::false_type) 
{
	if (ptr != nullptr)
	{
		ptr->~Ty();
	}
}

template<class ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

template<class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
{
	for (; first != last; ++first)
		destroy(&*first);
}

template<class Ty>
void destory(Ty* ptr)
{
	destroy_one(ptr, std::is_trivially_destructible<Ty>{})
}

template<class ForwardIter>
void destory(ForwardIter first, ForwardIter last)
{
	destroy_cat(first, last, std::is_trivially_destructible<
		typename iterator_traits<ForwardIter>::value_type>{});
}

}

