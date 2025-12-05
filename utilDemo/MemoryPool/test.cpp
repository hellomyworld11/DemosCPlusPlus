#include "MemoryPool.h"
#include "alloc.h"
#include <iostream>
#include <time.h>
#include <vector>


const int KCount = 1000000;



int main() {

	SimpleAlloc<int, std::allocator<int>> defaultalloc;
	clock_t start = clock();

	for (int i = 0; i < KCount; i++)
	{
		defaultalloc.push(i);
	}
	for (int i = 0; i < KCount; i++)
	{
		defaultalloc.pop();
	}
	std::cout << "Default Allocator Time: ";
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

	SimpleAlloc<int, MemoryPool<int>> pool;
	start = clock();
	for (int i = 0; i < KCount; i++)
	{
		pool.push(i);
	}
	for (int i = 0; i < KCount; i++)
	{
		pool.pop();
	}
	std::cout << "MemoryPool Allocator Time: ";
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

	std::vector<int> arr;
	start = clock();
	for (int i = 0; i < KCount; i++)
	{
		arr.push_back(i);
	}
	for (int i = 0; i < KCount; i++)
	{
		arr.pop_back();
	}
	std::cout << "Vector Time: ";
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

	getchar();
	return 0;
}