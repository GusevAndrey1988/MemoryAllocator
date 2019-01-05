#ifndef _IALLOCATOR_H
#define _IALLOCATOR_H

#include <cstddef>
#include <cassert>
#include <cstdint>
#include <utility>

using u8 = uint8_t;
using uptr = uintptr_t;

class IAllocator {
protected:
	void *_start;
	size_t _size;
	size_t _used_memory;
	size_t _num_allocations;
public:
	IAllocator(size_t size, void *start)
		: _start{start}, _size{size}, 
		_used_memory{0}, _num_allocations{0}
	{
	}

	virtual ~IAllocator()
	{
		assert(_num_allocations == 0 && _used_memory == 0);

		_start = nullptr;
		_size = 0;
	}

	virtual void* allocate(size_t size, u8 alignment = 4)=0;
	virtual void deallocate(void *p)=0;

	void* get_start() const { return _start; }
	size_t get_size() const { return _size; }
	size_t get_used_memory() const { return _used_memory; }
	size_t get_num_allocations() const { return _num_allocations; }
}; //Allocator

inline void* alignForward(void *address, u8 alignment)
{
	return reinterpret_cast<void*>(
		((reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment-1)) & 
		  static_cast<u8>(~(alignment-1)))
	);
}

inline u8 alignForwardAdjustement(const void *address, u8 alignment)
{
	u8 adjustment = 
		alignment - (reinterpret_cast<uptr>(address) & static_cast<uptr>(alignment-1));

	if (adjustment == alignment) return 0;

	return adjustment;
}

inline u8 alignForwardAdjustementWithHeader(
		const void *address, u8 alignment, u8 headerSize)
{
	u8 adjustment  = alignForwardAdjustement(address, alignment);
	u8 neededSpace = headerSize;

	if (adjustment < neededSpace) {
		neededSpace -= adjustment;

		adjustment += alignment * (neededSpace/alignment);
		
		if (neededSpace%alignment > 0) adjustment += alignment;
	}

	return adjustment;
}

inline void* add(void *address, size_t v)
{
	return reinterpret_cast<void*>(reinterpret_cast<uptr>(address) + v);
}

inline void* substract(void *address, size_t v)
{
	return reinterpret_cast<void*>(reinterpret_cast<uptr>(address) - v);
}

namespace allocator {

template<typename T, typename... Args> 
T* allocate_new(IAllocator &allocator, Args&&... args)
{
	return new (allocator.allocate(sizeof(T), alignof(T))) 
		T(std::forward<Args>(args)...);
}

template<typename T>
void deallocate_delete(IAllocator &allocator, T *object)
{
	object->~T();
	allocator.deallocate(object);
}

template<typename T>
T* allocate_array(IAllocator &allocator, size_t length)
{
	assert(length !=0);

	u8 header_size = sizeof(size_t)/sizeof(T);
	if (sizeof(size_t)%sizeof(T) > 0) header_size += 1;	

	T *p = reinterpret_cast<T*>(
			allocator.allocate(sizeof(T) * (length + header_size),
				alignof(T))) + header_size;

	*(reinterpret_cast<size_t*>(p) - 1) = length;

	for (size_t i = 0; i != length; ++i) {
		new (&p[i]) T;
	}

	return p;
}

template<typename T>
void deallocate_array(IAllocator &allocator, T *array)
{
	assert(array != nullptr);

	size_t length = *(reinterpret_cast<size_t*>(array) - 1);

	for (size_t i = 0; i != length; ++i) {
		array[i].~T();
	}

	u8 header_size = sizeof(size_t)/sizeof(T);
	if (sizeof(size_t)%sizeof(T) > 0) header_size += 1;

	allocator.deallocate(array - header_size);
}

} //allocator

#endif //_IALLOCATOR_H
