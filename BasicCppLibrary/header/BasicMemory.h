#pragma once

//TODO: more custom allocators, memory allocation strategies, tests...

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
		std::cout << std::endl << "Allocated " << size << " bytes at " << p;
#endif // _DEBUG

		return p;
	}

	throw std::bad_alloc{};
}

void operator delete(void* p)
{
#ifdef _DEBUG
	std::cout << std::endl << "Deallocated bytes at " << p;
#endif // _DEBUG
	free(p);
	p = nullptr;
}

namespace bsc
{
	class allocator_data
	{
	protected:
		static uint allocations;
		static uint deallocations;

	public:
		static uint num_allocations() { return allocations; }
		static uint num_deallocations() { return deallocations; }
	};

	uint allocator_data::allocations = 0;
	uint allocator_data::deallocations = 0;

	// base_allocator
	template<class T>
	class base_allocator 
#ifdef _DEBUG
		: public allocator_data
#endif // _DEBUG
	{
	public:
		using value_type								= T;
		using propagate_on_container_move_assignment	= std::true_type;
		using is_always_equal							= std::true_type;

		base_allocator() noexcept = default;
		base_allocator(const base_allocator&) noexcept = default;
		~base_allocator() = default;

		template<class U> 
		base_allocator(const base_allocator<U>&) noexcept {}

		T* allocate(std::size_t n);
		void deallocate(T* p, std::size_t);
	};

	template<class T>
	inline T* base_allocator<T>::allocate(std::size_t n)
	{
#ifdef _DEBUG
		++allocations;
#endif // _DEBUG
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	template<class T>
	inline void base_allocator<T>::deallocate(T* p, std::size_t n)
	{
#ifdef _DEBUG
		++deallocations;
#endif // _DEBUG
		::operator delete(p);
	}

	template<class T, class U>
	bool operator==(const base_allocator<T>&, const base_allocator<U>&) noexcept
	{
		return true;
	}

	template<class T, class U>
	bool operator!=(const base_allocator<T>&, const base_allocator<U>&) noexcept
	{
		return false;
	}

}