#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <array>

template<size_t MAX_ELEMENTS>
struct AllocatorInternals
{
	AllocatorInternals(size_t sizeofT)
	{
		m_memory = new uint8_t[MAX_ELEMENTS * sizeofT];
		m_refCount = 1;
		m_originalElementSize = sizeofT;
		for (auto& b : m_map)
			b = true; // mark as 'free'
	}
	AllocatorInternals(const AllocatorInternals&) = delete;
	~AllocatorInternals()
	{
		delete[] m_memory;
		m_memory = nullptr;
	}

	uint8_t *m_memory = nullptr;
	size_t m_refCount;
	size_t m_originalElementSize;
	std::array<bool, MAX_ELEMENTS> m_map;
};

template<class T, size_t MAX_ELEMENTS>
class PoolAllocator
{
	static_assert( !std::is_same_v<T, void>, "Type of the allocator can't be void" );
public:
	PoolAllocator()
	{
		m_internals = new AllocatorInternals<MAX_ELEMENTS>(sizeof(T));
	}

	~PoolAllocator()
	{
		m_internals->m_refCount--;
		if ( m_internals->m_refCount == 0 )
			delete m_internals;
	}

	PoolAllocator(const PoolAllocator& other)
	{
		m_internals = other.m_internals;
		m_internals->m_refCount++;
		// windows can copy-construct other allocator
	}

	template<class U>
	PoolAllocator(const PoolAllocator<U, MAX_ELEMENTS> &other) noexcept {

		m_internals = other.m_internals;
		m_internals->m_refCount++;
	}

	template<class U, size_t N>
	friend class PoolAllocator;

	using value_type = T;

	template <typename U>
	struct rebind
	{
		using other = PoolAllocator<U, MAX_ELEMENTS>;
	};

	T* allocate(std::size_t count_objects)
	{
		// windows can allocate:
		// - object with sizeof(U) < original sizeof(int) when constructing vector<int> - not handled
		// - object with sizeof(U) > sizeof(int) when constructing map<int, int>
		size_t i = 0;
		for (; i < MAX_ELEMENTS; i++)
		{
			if (m_internals->m_map[i] == false)
				continue;

			int canAllocate = 1;
			for (size_t j = i + 1; j < MAX_ELEMENTS; j++)
			{
				if (m_internals->m_map[j] == false)
				{
					i = j + 1;
					break; // fail, continue outer cycle
				}
				canAllocate++;
				if (canAllocate >= count_objects)
					break; // ok, found
			}
			if (canAllocate >= count_objects)
				break; // ok
		}
		if (i >= MAX_ELEMENTS)
			throw std::bad_alloc();
		
		// must allocate at m_memory + i*sizeof(T) .. m_memory + count_objects*sizeof(T)
		T* res = reinterpret_cast<T*>( m_internals->m_memory + i * sizeof(T) );
		for (size_t k = i; k < i + count_objects; k++)
			m_internals->m_map[k] = false; // mark as 'used'

		return res;
	}

	void deallocate(T* ptr, std::size_t count_objects)
	{
		size_t idx = (reinterpret_cast<uint8_t*>(ptr) - m_internals->m_memory) / sizeof(T);
		for (size_t i = idx; i < count_objects; i++)
			m_internals->m_map[i] = true; // mark as 'free'
	}

	template <typename U, typename... Args>
	void construct(U* p, Args&&... args)
	{
		new ( p ) U(std::forward<Args>(args)...);
	}

	void destroy(T* p)
	{
		p->~T();
	}

protected:
	AllocatorInternals<MAX_ELEMENTS> *m_internals = nullptr;
};
