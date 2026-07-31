#pragma once
#include "construct.h"

namespace TSL {


template<class ForwardIter, class T>
void unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type)
{
	TSL::fill(first, last, value);
}

template<class ForwardIter, class T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
{

}

template<class ForwardIter, class T>
void fill(ForwardIter first, ForwardIter last, const T& value)
{
	//fill_cat
}




}
