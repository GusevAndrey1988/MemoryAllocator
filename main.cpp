#define _DEBUG(true)

#include <iostream>
#include <memory>
#include <functional>
#include <numeric>
#include <algorithm>

#include "LinearAllocator.h"
#include "StackAllocator.h"

template<typename T>
void print(T val) 
{
	std::cout << val << std::endl;
}

template<typename T, typename... Args>
void print(T first, Args... arg)
{
	std::cout << first;
	print(arg...);
}

template<typename Iter>
void print_arr(Iter begin, Iter end)
{
	Iter current = begin;
	std::cout << "{";
	while (current != end) {
		if (current == begin) std::cout << *current;
		else std::cout << ", " << *current;
		++current;
	}
	std::cout << "}";
}

template<typename Cont>
void print_arr(Cont c)
{
	print_arr(c.begin, c.end);
}

struct Test_class {
	int data;
	
	Test_class() : data{++prefix} {  }; 
	int get() const { return data; }
private:
	static int prefix;
};

int Test_class::prefix = 0;

int main(int argc, char *argv[])
{
	constexpr size_t size = 1024;
	std::unique_ptr<char,std::function<void(char*)>> memory{new char [size],
		[](char *ptr) { delete[] ptr; }};

	StackAllocator sa{size, memory.get()};	

	int *a = allocator::allocate_new<int>(sa, 100);	
	std::cout << *a << std::endl;	
	int *b = allocator::allocate_array<int>(sa, 10);
	for (int i = 0; i != 10; ++i) {
		b[i] = i*i;
	}
	print_arr(b,b+10);
	std::cout << std::endl;

	allocator::deallocate_array(sa, b);
	allocator::deallocate_delete(sa, a);

	std::cout << *a << std::endl;	

	return 0;
}
