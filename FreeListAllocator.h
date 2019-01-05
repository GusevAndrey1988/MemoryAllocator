#ifndef _FREELISTALLOCATOR_H

#define _FREELISTALLOCATOR_H

#include "Allocator.h"

class FreeListAllocator : public IAllocator {
private:
	struct AllocationHeader { size_t size; u8 adjustment; };
	struct FreeBlock { size_t size; FreeBlock *next; };

	FreeBlock *_free_blocks;

	FreeListAllocator(const FreeListAllocator &)=delete;
	FreeListAllocator& operator=(const FreeListAllocator &);
public:
	FreeListAllocator(size_t size, void *start);
	~FreeListAllocator();

	void* allocate(size_t size, u8 alignment) override;
	void deallocate(void *p) override;
}; //FreeListAllocator

#endif //_FREELISTALLOCATOR_H
