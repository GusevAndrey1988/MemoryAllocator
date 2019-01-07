#include "PoolAllocator.h"

#include <cassert>

PoolAllocator::PoolAllocator(size_t objectSize, u8 objectAlignment, size_t size, void *mem)
	: IAllocator{size, mem}, _objectSize{objectSize}, _objectAlignment{objectAlignment}
{
	assert(objectSize >= sizeof(void*));

	u8 adjustment = alignForwardAdjustement(mem, objectAlignment);
	_free_list = static_cast<void**>(add(mem, adjustment));
	size_t numObjects = (size - adjustment) / objectSize;
	void **p = _free_list;

	for (size_t i = 0; i < numObjects-1; ++i) {
		*p = add(p, objectSize);
		p = static_cast<void**>(*p);
	}

	*p = nullptr;
}

PoolAllocator::~PoolAllocator()
{
	_free_list = nullptr;
}

void* PoolAllocator::allocate(size_t size, u8 alignment)
{
	assert(size == _objectSize && alignment == _objectAlignment);

	if (_free_list == nullptr) return nullptr;
	void *p = _free_list;
	_free_list = static_cast<void**>(*_free_list);
	_used_memory += size;
	++_num_allocations;

	return p;
}

void PoolAllocator::deallocate(void *p)
{
	*((void**)p) = _free_list;
	_free_list = (void**)p;
	_used_memory -= _objectSize;
	--_num_allocations;
}
