#pragma once

#include <thread>
#include <atomic>
#include <mutex>

namespace bsc
{
	// FIFO single producer, single consumer
	template<typename T>
	class lock_free_queue
	{
		struct Node
		{
			T value;
			Node* next;

			Node(const T& val) : value(val), next(nullptr) {}
		};

		Node* m_first; // First node of the queue
		Node* m_divider; // Divider between consumed and unconsumed nodes
		Node* m_last; // Last added node

	public:

		lock_free_queue() : m_first(nullptr), m_divider(new Node(0)), m_last(nullptr) 
		{
			// Divider starts with a dummy node separator
			m_first = m_divider;
			m_last = m_divider;
		}

		~lock_free_queue()
		{
			while (m_first != nullptr)
			{
				Node* temp = m_first;
				m_first = m_first->next;
				delete temp;
			}
		}

		bool produce(const T& value)
		{
			m_last->next = new Node(value);
			m_last = m_last->next;

			// Delete consumed nodes, they are owned by the producer
			// and to the left of the divider node, which is always valid
			while (m_first != m_divider)
			{
				Node* temp = m_first;
				m_first = m_first->next;
				delete temp;
			}

			return true;
		}

		bool consume(T& out_result)
		{
			// Empty queue check, the divider node is already consumed
			if (m_divider != m_last)
			{         
				out_result = m_divider->next->value;
				m_divider = m_divider->next;
				return true;
			}
			return false;
		}
	};

	// FIFO single producer, single consumer
	template<typename T>
	class lock_free_ring_buffer
	{
		T* m_data;
		size_t m_capacity;
		std::atomic<size_t> m_front; // First used space
		std::atomic<size_t> m_tail; // Space after the last used, always after front unless empty

	public:

		lock_free_ring_buffer(size_t capacity) : m_capacity(capacity + 1), m_data(new T[capacity + 1]), m_front(0), m_tail(0) {}
		~lock_free_ring_buffer()
		{
			delete[] m_data;
		}

		size_t capacity() const { return m_capacity - 1; }
		size_t size() const
		{
			const size_t tail = m_tail.load(std::memory_order_relaxed);
			const size_t front = m_front.load(std::memory_order_relaxed);

			if (tail >= front)
			{
				return tail - front;
			}
			else
			{
				return capacity - front + tail;
			}
		}

		bool produce(const T& value)
		{
			//Owns m_tail
			size_t tail = m_tail.load(std::memory_order_relaxed);
			size_t next = (tail + 1) % m_capacity;

			// Buffer is not full, we don't use the last space
			if (next != m_front.load(std::memory_order_relaxed))
			{
				m_data[tail] = value;
				m_tail.store(next, std::memory_order_release);
				return true;
			}

			return false;
		}

		bool consume(T& out_result)
		{
			// Owns m_front
			size_t front = m_front.load(::std::memory_order_relaxed);
			if (front != m_tail.load(std::memory_order_acquire)) // Empty buffer
			{
				out_result = m_data[front];
				m_data[front] = 0;
				m_front.store((front + 1) % m_capacity, std::memory_order_release);
				return true;
			}

			return false;
		}

	};

	// TODO: multi-producer, multi-consumer lock free queue
}