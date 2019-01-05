#ifndef _LINEARALLOCATOR_H
#define _LINEARALLOCATOR_H

#include "Allocator.h"

class LinearAllocator : public IAllocator {
private:
	LinearAllocator(const LinearAllocator &)=delete;
	LinearAllocator& operator=(const LinearAllocator &)=delete;

	void *_current_pos;
public:
	LinearAllocator(size_t size, void *start);
	~LinearAllocator();

	void* allocate(size_t size, u8 alignment) override;
	void deallocate(void *p) override;

	void clear();
}; //LinearAllocator

#endif //_LINEARALLOCATOR_H
