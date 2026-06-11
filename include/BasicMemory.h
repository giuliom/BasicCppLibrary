#pragma once

#include <cassert>
#include <cstddef>
#include <atomic>
#include <utility>
#include <BasicGlobal.h>

//TODO: more custom allocators, memory allocation strategies, tests...


namespace bsc
{
	// ======================================= ALLOCATORS =======================================

	// ----- allocator_data -----
	class allocator_data
	{
	protected:
		static std::atomic<long> allocations;
		static std::atomic<long> deallocations;

	public:
		static long num_allocations() { return allocations; }
		static long num_deallocations() { return deallocations; }
	};

	// Static member definitions
	inline std::atomic<long> allocator_data::allocations = 0;
	inline std::atomic<long> allocator_data::deallocations = 0;

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
	//TODO create common base class for shared_ptr and weak_ptr


	// ----- unique_ptr -----
	template<class T>
	class unique_ptr
	{
		T* m_ptr;

	public:
		unique_ptr() noexcept : m_ptr(nullptr) {}
		constexpr unique_ptr(std::nullptr_t) noexcept : m_ptr(nullptr) {}
		explicit unique_ptr(T* ptr) noexcept : m_ptr(ptr) {}
		
		~unique_ptr()
		{
			cleanup();
		}

		unique_ptr(const unique_ptr& other) = delete;
		unique_ptr& operator=(const unique_ptr& other) = delete;

		unique_ptr(unique_ptr&& other) noexcept
		{
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}

		unique_ptr& operator=(unique_ptr&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			cleanup();

			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			return *this;
		}

		void reset(T* ptr = nullptr) noexcept
		{
			T* old = m_ptr;
			m_ptr = ptr;
			delete old;
		}

		T* release() noexcept
		{
			T* ptr = m_ptr;
			m_ptr = nullptr;
			return ptr;
		}

		void swap(unique_ptr& other) noexcept
		{
			T* tmp = m_ptr;
			m_ptr = other.m_ptr;
			other.m_ptr = tmp;
		}

		explicit operator bool() const noexcept
		{
			return m_ptr != nullptr;
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
			return m_ptr;
		}

	private:
		inline void cleanup()
		{
			delete m_ptr;
			m_ptr = nullptr;
		}
	};

	// ----- make_unique -----
	template<class T, class... Args>
	unique_ptr<T> make_unique(Args&&... args)
	{
		return unique_ptr<T>(new T(std::forward<Args>(args)...));
	}


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

		std::atomic<long> m_resource_ref_count;
		std::atomic<long> m_block_ref_count;
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
#ifdef _DEBUG
			assert(m_resource_ref_count.load() == 0);
			assert(m_block_ref_count.load() == 0);
#endif // _DEBUG
		}

		void increment_res_count()
		{
			m_block_ref_count.fetch_add(1, std::memory_order_relaxed);
			m_resource_ref_count.fetch_add(1, std::memory_order_relaxed);
		}

		// Increments the resource count only if the resource is still alive.
		// Used by weak_ptr::lock() to avoid the expired-check/increment race.
		bool try_increment_res_count()
		{
			long count = m_resource_ref_count.load(std::memory_order_relaxed);
			do
			{
				if (count == 0)
				{
					return false;
				}
			} while (!m_resource_ref_count.compare_exchange_weak(count, count + 1,
					std::memory_order_acq_rel, std::memory_order_relaxed));

			m_block_ref_count.fetch_add(1, std::memory_order_relaxed);
			return true;
		}

		void decrement_res_count()
		{
			if (m_resource_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				delete m_resource;
				m_resource = nullptr;
			}

			decrement_block_count();
		}

		void increment_block_count()
		{
			m_block_ref_count.fetch_add(1, std::memory_order_relaxed);
		}

		void decrement_block_count()
		{
			if (m_block_ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				delete this;
			}
		}

		long get_res_count() const
		{
			return m_resource_ref_count.load(std::memory_order_relaxed);
		}

		long get_block_count() const
		{
			return m_block_ref_count.load(std::memory_order_relaxed);
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
		constexpr shared_ptr(std::nullptr_t) noexcept : m_cblock(nullptr) {}
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
			if (this == &other)
			{
				return *this;
			}

			if (m_cblock != nullptr)
			{
				m_cblock->decrement_res_count();
			}

			m_cblock = other.m_cblock;

			if (m_cblock != nullptr)
			{
				m_cblock->increment_res_count();
			}

			return *this;
		}

		shared_ptr(shared_ptr&& other) noexcept
		{
			m_cblock = other.m_cblock;
			other.m_cblock = nullptr;
		}

		shared_ptr& operator=(shared_ptr&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			// Take other's reference first, then release our own. This is
			// correct even when both point at the same control block: the
			// two references collapse into one.
			control_block<T>* old = m_cblock;
			m_cblock = other.m_cblock;
			other.m_cblock = nullptr;

			if (old != nullptr)
			{
				old->decrement_res_count();
			}

			return *this;
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
			return m_cblock ? m_cblock->m_resource : nullptr;
		}

		explicit operator bool() const noexcept
		{
			return get() != nullptr;
		}

		void swap(shared_ptr& other) noexcept
		{
			control_block<T>* tmp = m_cblock;
			m_cblock = other.m_cblock;
			other.m_cblock = tmp;
		}

		long get_count() const
		{
			return m_cblock ? m_cblock->get_res_count() : 0;
		}
	};

	// ----- make_shared -----
	template<class T, class... Args>
	shared_ptr<T> make_shared(Args&&... args)
	{
		return shared_ptr<T>(new T(std::forward<Args>(args)...));
	}


	// ----- weak_ptr -----
	template<class T>
	class weak_ptr
	{
		control_block<T>* m_cblock;

	public:
		weak_ptr() : m_cblock(nullptr) {}
		constexpr weak_ptr(std::nullptr_t) noexcept : m_cblock(nullptr) {}
		weak_ptr(const shared_ptr<T>& shared) : m_cblock(shared.m_cblock)
		{
			if (m_cblock != nullptr)
			{
				m_cblock->increment_block_count();
			}
		}
		weak_ptr(const weak_ptr& other) : m_cblock(other.m_cblock)
		{
			if (m_cblock != nullptr)
			{
				m_cblock->increment_block_count();
			}
		}
		weak_ptr(weak_ptr&& other) noexcept : m_cblock(other.m_cblock)
		{
			other.m_cblock = nullptr;
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
			if (this == &other)
			{
				return *this;
			}

			if (m_cblock != nullptr)
			{
				m_cblock->decrement_block_count();
			}

			m_cblock = other.m_cblock;

			if (m_cblock != nullptr)
			{
				m_cblock->increment_block_count();
			}

			return *this;
		}

		weak_ptr& operator=(weak_ptr&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			control_block<T>* old = m_cblock;
			m_cblock = other.m_cblock;
			other.m_cblock = nullptr;

			if (old != nullptr)
			{
				old->decrement_block_count();
			}

			return *this;
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

		shared_ptr<T> lock() const
		{
			shared_ptr<T> sp;

			if (m_cblock != nullptr && m_cblock->try_increment_res_count())
			{
				sp.m_cblock = m_cblock;
			}

			return sp;
		}

		void swap(weak_ptr<T>& b)
		{
			control_block<T>* tmp = m_cblock;
			m_cblock = b.m_cblock;
			b.m_cblock = tmp;
		}
	};
}