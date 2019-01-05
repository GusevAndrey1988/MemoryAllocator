#include "LinearAllocator.h"

LinearAllocator::LinearAllocator(size_t size, void *start) 
	: IAllocator{size, start}, _current_pos(start)
{
	assert(size > 0);
}

LinearAllocator::~LinearAllocator()
{
	_current_pos = nullptr;
}

void* LinearAllocator::allocate(size_t size, u8 alignment)
{
	assert(size != 0);

	u8 adjustment = alignForwardAdjustement(_current_pos, alignment);

	if (_used_memory + adjustment + size > _size) return nullptr;

	uptr aligned_address = reinterpret_cast<uptr>(_current_pos) + adjustment;
	_current_pos = reinterpret_cast<void*>(aligned_address + size);
	_used_memory += size + adjustment;
	++_num_allocations;

	return reinterpret_cast<void*>(aligned_address);
}

void LinearAllocator::deallocate(void *p)
{
	assert(false && "Use clear() instead");
}

void LinearAllocator::clear() 
{
	_num_allocations = 0;
	_used_memory = 0;
	_current_pos = _start;
}
