#ifndef _POOLALLOCATOR_H
#define _POOLALLOCATOR_H

#include "Allocator.h"

class PoolAllocator : public IAllocator {
private:
	PoolAllocator(const PoolAllocator &)=delete;
	PoolAllocator& operator=(const PoolAllocator &)=delete;

	size_t _objectSize;
	u8 _objectAlignment;
	void **_free_list;
public:
	PoolAllocator(size_t objectSize, u8 objectAlignment, size_t size, void *mem);
	~PoolAllocator();

	void* allocate(size_t size, u8 alignment) override;
	void deallocate(void *p) override;
}; //PoolAllocator

#endif //_POOLALLOCATOR_H
