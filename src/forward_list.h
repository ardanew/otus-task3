#pragma once
#include <memory>
#include <functional>

template<typename T, typename Allocator = std::allocator<T>>
class ForwardList
{
protected:
	struct Node
	{
		T m_value;
		Node* m_next = nullptr;
	};
	Node *m_head = nullptr;
	typename Allocator::template rebind<Node>::other m_allocator;

public:
	~ForwardList() 
	{
		Node* ptr = m_head;
		while (true)
		{
			Node* next = ptr->m_next;

			m_allocator.destroy(ptr);
			m_allocator.deallocate(ptr, 1);

			if (next == nullptr)
				break;
			ptr = next;
		}
	}

	void addNew(T elem)
	{
		if (m_head == nullptr)
		{
			m_head = m_allocator.allocate(1);
			m_allocator.construct(m_head);
			m_head->m_value = std::move(elem);
			return;
		}

		Node* ptr = m_head;
		while (ptr->m_next != nullptr)
			ptr = ptr->m_next;
		ptr->m_next = m_allocator.allocate(1);
		m_allocator.construct(ptr->m_next);
		ptr->m_next->m_value = std::move(elem);
	}

	void forEach(std::function<void(T&)> &&f)
	{
		Node* ptr = m_head;
		while (ptr != nullptr)
		{
			f(ptr->m_value);
			ptr = ptr->m_next;
		}
	}
};