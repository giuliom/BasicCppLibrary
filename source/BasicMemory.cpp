#include <BasicMemory.h>

// ----- custom global new -----
void* operator new(std::size_t size)
{
	if (size == 0) // Must always allocate at least one byte
	{
		++size;
	}

	void* p = malloc(size);
	if (p)
	{
#ifdef _DEBUG
//		std::cout << std::endl << "Allocated " << size << " bytes at " << p;
#endif // _DEBUG

		return p;
	}

	throw std::bad_alloc{};
}


// ----- custom global delete -----
void operator delete(void* p) noexcept
{
#ifdef _DEBUG
//	std::cout << std::endl << "Deallocated bytes at " << p;
#endif // _DEBUG
	free(p);
}

namespace bsc
{
    std::atomic<long> allocator_data::allocations = 0;
	std::atomic<long> allocator_data::deallocations = 0;
}