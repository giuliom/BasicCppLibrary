#pragma once

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
		size_t m_front; // First used space
		size_t m_tail; // Space after the last used, always after front unless empty

	public:

		lock_free_ring_buffer(size_t capacity) : m_capacity(capacity + 1), m_data(new T[capacity + 1]), m_front(0), m_tail(0) {}
		~lock_free_ring_buffer()
		{
			delete[] m_data;
		}

		size_t capacity() const { return m_capacity - 1; }
		size_t size() const
		{
			if (m_tail >= m_front)
			{
				return m_tail - m_front;
			}
			else
			{
				m_capacity - m_front + m_tail;
			}
		}

		bool produce(const T& value)
		{
			//Owns m_tail
			size_t next = (m_tail + 1) % m_capacity;

			// Buffer is not full, we don't use the last space
			if (next != m_front)
			{
				m_data[m_tail] = value;
				// Memory Barrier HERE
				m_tail = next;
				return true;
			}

			return false;
		}

		bool consume(T& out_result)
		{
			// Owns m_front
			if (m_front != m_tail) // Empty buffer
			{
				out_result = m_data[m_front];
				m_data[m_front] = 0;
				// Memory Barrier HERE
				m_front = (m_front + 1) % m_capacity;
				return true;
			}

			return false;
		}

	};

	// TODO: multi-producer, multi-consumer lock free queue
}