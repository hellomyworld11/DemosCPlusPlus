#pragma once

namespace TSL{


template<class T>
const T& max(const T& lhs, const T& rhs)
{
	return lhs < rhs ? rhs : lhs;
}

template<class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare compare)
{
	return compare(lhs, rhs) ? rhs : lhs;
}

template<class T>
const T& min(const T& lhs, const T& rhs)
{
	return rhs < lhs ? rhs : lhs;
}

template<class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare compare)
{
	return compare(lhs, rhs) ? lhs : rhs;
}

}
