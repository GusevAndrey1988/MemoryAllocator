#include "StackAllocator.h"
#include <iostream>

StackAllocator::StackAllocator(size_t size, void *start)
	: IAllocator{size, start}, _current_pos{start}
{
	assert(size > 0);
#if _DEBUG
	_prev_position = nullptr;
#endif
}

StackAllocator::~StackAllocator()
{
#if _DEBUG
	_prev_position = nullptr;
#endif

	_current_pos = nullptr;
}

void* StackAllocator::allocate(size_t size, u8 alignment)
{
	assert(size != 0);
	u8 adjustment = 
		alignForwardAdjustementWithHeader(_current_pos, alignment, sizeof(AllocationHeader));

	if (_used_memory + adjustment + size > _size) return nullptr;

	void *aligned_address = add(_current_pos, adjustment);

	AllocationHeader *header = static_cast<AllocationHeader*>(
			substract(aligned_address, sizeof(AllocationHeader)));
	header->adjustment = adjustment;

#if _DEBUG
	header->prev_address = _prev_position;
	_prev_position = aligned_address;
#endif

	_current_pos = reinterpret_cast<void*>(
			reinterpret_cast<uptr>(aligned_address) + size);
	_used_memory += size + adjustment;
	++_num_allocations;

	return aligned_address;
}

void StackAllocator::deallocate(void *p)
{
#if _DEBUG
	assert(p == _prev_position);
#endif

	AllocationHeader *header = static_cast<AllocationHeader*>(
			substract(p, sizeof(AllocationHeader)));
	_used_memory -= reinterpret_cast<uptr>(_current_pos) - reinterpret_cast<uptr>(p)
		+ header->adjustment;
	_current_pos = substract(p, header->adjustment);

#if _DEBUG
	_prev_position = header->prev_address;
#endif

	--_num_allocations;
}
