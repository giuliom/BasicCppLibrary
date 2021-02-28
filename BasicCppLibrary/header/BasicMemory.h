#pragma once

//TODO: more custom allocators, memory allocation strategies, tests...


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
		std::cout << std::endl << "Allocated " << size << " bytes at " << p;
#endif // _DEBUG

		return p;
	}

	throw std::bad_alloc{};
}


// ----- custom global delete -----
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
	// ======================================= ALLOCATORS =======================================

	// ----- allocator_data -----
	class allocator_data
	{
	protected:
		static long allocations;
		static long deallocations;

	public:
		static long num_allocations() { return allocations; }
		static long num_deallocations() { return deallocations; }
	};

	long allocator_data::allocations = 0;
	long allocator_data::deallocations = 0;


	// ----- base_allocator -----
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
		virtual ~base_allocator() = default;

		template<class U> 
		base_allocator(const base_allocator<U>&) noexcept {}

		virtual T* allocate(std::size_t n);
		virtual void deallocate(T* p, std::size_t);
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


	// ----- stack_memory -----
	class stack_memory
	{
		uchar*		m_buf;
		std::size_t m_buf_size;
		std::size_t m_offset;

	public:
		stack_memory(uchar* buf, std::size_t buf_size) 
			: m_buf(buf)
			, m_buf_size(buf_size)
			, m_offset(0)
		{}
	};

	// ======================================= SMART POINTERS =======================================

	//TODO write rest of the class and T[] version
	//TODO write make_() functions
	//TODO implement atomic operations
	//TODO create common base class for shared_ptr and weak_ptr
	//TODO testing


	// ----- unique_ptr -----
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

		void reset()
		{
			cleanup();
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
			m_ptr = nullptr;
		}
	};


	template<class T>
	class shared_ptr;

	template< class T>
	class weak_ptr;

	// ----- control_block -----
	template<class T>
	class control_block
	{
		friend class shared_ptr<T>;
		friend class weak_ptr<T>;

		long m_resource_ref_count;
		long m_block_ref_count;
		T* m_resource;

		control_block(T* resource)
			: m_resource_ref_count(1)
			, m_block_ref_count(1)
			, m_resource(resource)
		{}

		control_block(const control_block& other) = delete;
		control_block& operator=(const control_block& other) = delete;

		~control_block()
		{
			delete m_resource;

#ifdef _DEBUG
			assert(m_resource_ref_count == 0);
			assert(m_block_ref_count == 0);
#endif // _DEBUG
		}

		inline void cleanup()
		{
#ifdef _DEBUG
			if (m_block_ref_count == 0)
			{
				assert(m_resource_ref_count == 0);
			}
#endif // _DEBUG

			if (m_resource_ref_count == 0)
			{
				delete m_resource;
				m_resource = nullptr;
			}

			if (m_block_ref_count == 0)
			{
				delete this;
			}
		}

		void increment_res_count()
		{
			++m_block_ref_count;
			++m_resource_ref_count;
		}

		void decrement_res_count()
		{
			--m_block_ref_count;
			--m_resource_ref_count;
			cleanup();
		}

		void increment_block_count()
		{
			++m_block_ref_count;
		}

		void decrement_block_count()
		{
			--m_block_ref_count;
			cleanup();
		}

		long get_res_count() const
		{
			return m_resource_ref_count;
		}

		long get_block_count() const
		{
			return m_block_ref_count;
		}
	};

	// ----- shared_ptr -----
	template<class T>
	class shared_ptr
	{
		friend class weak_ptr<T>;

		control_block<T>* m_cblock;

	public:
		shared_ptr() : m_cblock(nullptr) {}
		shared_ptr(T* ptr) : m_cblock(new control_block<T>(ptr)) {}

		~shared_ptr()
		{
			if (m_cblock != nullptr)
			{
				m_cblock->decrement_res_count();
			}
		}

		shared_ptr(const shared_ptr& other)
		{
			m_cblock = other.m_cblock;

			if (m_cblock != nullptr)
			{
				m_cblock->increment_res_count();
			}
		}

		shared_ptr& operator=(const shared_ptr& other)
		{
			if (m_cblock != nullptr)
			{
				m_cblock->decrement_res_count();
			}

			m_cblock = other.m_cblock;

			if (m_cblock != nullptr)
			{
				m_cblock->increment_res_count();
			}
		}

		shared_ptr(shared_ptr&& other)
		{
			m_cblock = other.m_cblock;
			other.m_cblock = nullptr;
		}

		shared_ptr& operator=(shared_ptr&& other)
		{
			if (m_cblock != other.m_cblock)
			{
				m_cblock->decrement_res_count();
			}

			m_cblock = other.m_cblock;
			other.m_cblock = nullptr;
		}

		void reset()
		{
			if (m_cblock != nullptr)
			{
				m_cblock->decrement_res_count();
				m_cblock = nullptr;
			}
		}

		T* operator->() const
		{
			return m_cblock->m_resource;
		}

		T& operator*() const
		{
			return *m_cblock->m_resource;
		}

		T* get() const noexcept
		{
			if (m_cblock)
			{
				return m_cblock->m_resource;
			}

			return nullptr;
		}

		long get_count() const
		{
			return m_cblock->get_res_count();
		}
	};


	// ----- weak_ptr -----
	template<class T>
	class weak_ptr
	{
		control_block<T>* m_cblock;

	public:
		weak_ptr() : m_cblock(nullptr) {}
		weak_ptr(shared_ptr<T> shared) : m_cblock(shared.m_cblock)
		{
			if (m_cblock != nullptr)
			{
				m_cblock->increment_block_count();
			}
		}

		~weak_ptr()
		{
			if (m_cblock != nullptr)
			{
				m_cblock->decrement_block_count();
			}
		}

		weak_ptr& operator=(weak_ptr<T> const& other)
		{
			if (m_cblock != nullptr)
			{
				m_cblock->decrement_block_count();
			}

			m_cblock = other.m_cblock;

			if (m_cblock != nullptr)
			{
				m_cblock->increment_block_count();
			}
		}

		void reset()
		{
			if (m_cblock != nullptr)
			{
				m_cblock->decrement_block_count();
				m_cblock = nullptr;
			}
		}

		T* operator->() const
		{
			return m_cblock->m_resource;
		}

		T& operator*() const
		{
			return *m_cblock->m_resource;
		}

		T* get() const noexcept
		{
			if (m_cblock)
			{
				return m_cblock->m_resource;
			}

			return nullptr;
		}

		long get_count() const
		{
			return m_cblock ? m_cblock->get_res_count() : 0;
		}

		bool expired() const
		{
			return m_cblock == nullptr || m_cblock->get_res_count() == 0;
		}

		// TODO
		shared_ptr<T> lock() const {}
		void swap(weak_ptr<T>& b) {}
	};
}