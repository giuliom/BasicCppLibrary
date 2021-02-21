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

	//TODO write rest of the class and T[] version
	template<class T>
	class unique_ptr
	{
		T* m_ptr;

	public:
		unique_ptr() : ptr(nullptr) {}
		unique_ptr(T* ptr) : m_ptr(ptr) {}
		
		~unique_ptr()
		{
			cleanup();
		}

		unique_ptr(const unique_ptr& other) = delete;
		unique_ptr& operator=(const unique_ptr& other) = delete;

		unique_ptr(unique_ptr&& other)
		{
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}

		unique_ptr& operator=(unique_ptr&& other)
		{
			cleanup();

			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			return *this;
		}

		T* operator->() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		T* get() const noexcept
		{
			if (m_ptr)
			{
				return *m_ptr;
			}

			return nullptr;
		}

	private:
		inline void cleanup()
		{
			delete m_ptr;
		}
	};

	template<class T>
	class shared_ptr
	{
		T* m_ptr;
		uint* m_refcount;

	public:
		shared_ptr() : m_ptr(nullptr), m_refcount(new uint(0u)) {}
		shared_ptr(T* ptr) : m_ptr(ptr), m_refcount(new uint(1u)) {}

		~shared_ptr()
		{
			(*m_refcount)--;
			cleanup();
		}

		shared_ptr(const shared_ptr& other)
		{
			m_ptr = other.m_ptr;
			m_refcount = other.m_refcount;

			if (m_ptr != nullptr)
			{
				(*m_refcount)++;
			}
		}

		shared_ptr& operator=(const shared_ptr& other)
		{
			(*m_refcount)--;

			if (m_ptr != other.m_ptr)
			{
				cleanup();
			}

			m_ptr = other.m_ptr;
			m_refcount = other.m_refcount;

			if (m_ptr != nullptr)
			{
				(*m_refcount)++;
			}
		}

		shared_ptr(shared_ptr&& other)
		{
			m_ptr = other.m_ptr;
			m_refcount = other.m_refcount;

			other.m_ptr = nullptr;
			other.m_refcount = nullptr;
		}

		shared_ptr& operator=(shared_ptr&& other)
		{
			if (m_ptr != other.m_ptr)
			{
				(*m_refcount)--;
				cleanup();
			}

			m_ptr = other.m_ptr;
			m_refcount = other.m_refcount;

			other.m_ptr = nullptr;
			other.m_refcount = nullptr;
		}

		T* operator->() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		T* get() const noexcept
		{
			if (m_ptr)
			{
				return *m_ptr;
			}

			return nullptr;
		}

		uint get_count() const
		{
			return *m_refcount;
		}

	private:
		inline void cleanup()
		{
			if (*m_refcount == 0)
			{
				delete m_ptr;
				delete m_refcount;
			}
		}
	};


	// TODO weak_ptr
}