#include "FreeListAllocator.h"

#include <cassert>

FreeListAllocator::FreeListAllocator(size_t size, void *start) 
	: IAllocator{size, start}, _free_blocks(static_cast<FreeBlock*>(start))
{
	assert(size > sizeof(FreeBlock));
	_free_blocks->size = size;
	_free_blocks->next = nullptr;	
}

FreeListAllocator::~FreeListAllocator()
{
	_free_blocks = nullptr;
}

void* FreeListAllocator::allocate(size_t size, u8 alignment)
{
	assert(size != 0 && alignment != 0);

	FreeBlock *prev_free_block = nullptr;
	FreeBlock *free_block = _free_blocks;

	while (free_block != nullptr) {
		u8 adjustment = 
			alignForwardAdjustementWithHeader(free_block, alignment, sizeof(AllocationHeader));
		size_t total_size = size + adjustment;

		if (free_block->size < total_size) {
			prev_free_block = free_block;
			free_block = free_block->next;
			continue;
		}

	static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), 
			"sizeof(AllocationHeader) < sizeof(FreeBlock)");

		if (free_block->size - total_size <= sizeof(AllocationHeader))
		{
			total_size = free_block->size;

			if (prev_free_block != nullptr)
				prev_free_block->next = free_block->next;
			else
				_free_blocks = free_block->next;	
		} else {
			FreeBlock *next_block = static_cast<FreeBlock*>(add(free_block,total_size));

			next_block->size = free_block->size - total_size;
			next_block->next = free_block->next;

			if (prev_free_block != nullptr) 
				prev_free_block->next = next_block;
			else
				_free_blocks = next_block;
		}

		uptr aligned_address = reinterpret_cast<uptr>(free_block) + adjustment;
		AllocationHeader *header = 
			reinterpret_cast<AllocationHeader*>(aligned_address - sizeof(AllocationHeader));
		header->size = total_size;
		header->adjustment = adjustment;
		_used_memory += total_size;
		++_num_allocations;

		assert(alignForwardAdjustement(reinterpret_cast<void*>(aligned_address), alignment) == 0);

		return reinterpret_cast<void*>(aligned_address);
	}

	return nullptr;
}

void FreeListAllocator::deallocate(void *p)
{
	assert(p != nullptr);

	AllocationHeader *header = static_cast<AllocationHeader*>(substract(p, sizeof(AllocationHeader)));
	uptr block_start = reinterpret_cast<uptr>(p) - header->adjustment;
	size_t block_size = header->size;
	uptr block_end = block_start + block_size;
	FreeBlock *prev_free_block = nullptr;
	FreeBlock *free_block = _free_blocks;

	while (free_block != nullptr) {
		if (reinterpret_cast<uptr>(free_block) >= block_end) break;
		prev_free_block = free_block;
		free_block = free_block->next;
	}

	if (prev_free_block == nullptr) {
		prev_free_block = reinterpret_cast<FreeBlock*>(block_start);
		prev_free_block->size = block_size;
		prev_free_block->next = _free_blocks;
		_free_blocks = prev_free_block;
	} else if (reinterpret_cast<uptr>(prev_free_block) + prev_free_block->size == block_start) {
		prev_free_block->size += block_size;
	} else {
		FreeBlock *temp = reinterpret_cast<FreeBlock*>(block_start);
		temp->size = block_size;
		temp->next = prev_free_block->next;
		prev_free_block->next = temp;
		prev_free_block = temp;
	}

	assert(prev_free_block != nullptr);

	if (reinterpret_cast<uptr>(prev_free_block) + prev_free_block->size ==
			reinterpret_cast<uptr>(prev_free_block->next))
	{
		prev_free_block->size += prev_free_block->next->size;
		prev_free_block->next = prev_free_block->next->next;
	}

	--_num_allocations;
	_used_memory -= block_size;
}

