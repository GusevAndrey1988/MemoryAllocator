#ifndef _STACKALLOCATOR_H
#define _STACKALLOCATOR_H

#include "Allocator.h"

class StackAllocator : public IAllocator {
private:
	StackAllocator(const StackAllocator &)=delete;
	StackAllocator& operator=(const StackAllocator &)=delete;

	struct AllocationHeader {
	#if _DEBUG
		void *prev_address;
	#endif

		u8 adjustment;
	};

#if _DEBUG
	void *_prev_position;
#endif

	void *_current_pos;
public:
	StackAllocator(size_t size, void *start);
	~StackAllocator();

	void *allocate(size_t size, u8 alignment) override;
	void deallocate(void *p) override;
}; //StackAllocator

#endif //_STACKALLOCATOR_H
