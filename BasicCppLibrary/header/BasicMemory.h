#pragma once


void* operator new(std::size_t size)
{
	if (size == 0) // Must always allocate something
	{
		++size;
	}

	void* p = malloc(size);
	if (p)
	{
		//std::cout << "Allocated " << size << " bytes at " << p << std::endl;
		return p;
	}

	throw std::bad_alloc{};
}

void operator delete(void* p)
{
	//std::cout << "Memory at address " << p << " freed" << std::endl;
	free(p);
}

namespace bsc
{


}