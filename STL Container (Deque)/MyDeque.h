#pragma once
#include <memory>
#include <vector>
#include "RandomAccessIt.h"

namespace mystd
{
	template<class T, class Allocator = std::allocator<T>>
	class deque
	{
		typedef std::vector<T, Allocator> chunk;
		typedef typename std::allocator_traits<Allocator>::
			template rebind_alloc<chunk> chunkAllocator;
	public:
		//types
		using value_type = T;
		using allocator_type = Allocator;

		using pointer = typename std::allocator_traits<Allocator>::pointer;
		using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

		using reference = T&;
		using const_reference = const T&;

		using difference_type = typename std::allocator_traits<Allocator>::difference_type;
		using size_type = typename std::allocator_traits<Allocator>::size_type;

		using iterator = typename random_access_iterator<T, difference_type, pointer, reference, Allocator>;

		void show();

		deque(size_t n = 0, const Allocator& alloc = Allocator());
		deque(size_t, const value_type&, const Allocator& alloc = Allocator());

		template<class InputIt> 
			deque(InputIt first, InputIt last, const Allocator& alloc = Allocator());

		deque(const std::initializer_list<value_type>&, const Allocator& alloc = Allocator());

		~deque();

		size_t size();
		bool empty() const { return start == finish; }

		iterator begin() { return start; }
		iterator end() { return finish; }

		void push_front(const value_type&);
		void push_front(value_type&&);
		void push_back(const value_type&);
		void push_back(value_type&&);

		//using const_iterator
		//using reverse_iterator = std::reverse_iterator<iterator>;
		//using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	private:
		std::vector<chunk*> chunk_map;
		size_t chunk_size = 8;

		chunkAllocator chunk_alloc;
		Allocator data_alloc;

		iterator start;
		iterator finish;

		void createMap(size_t);
		void initChunks(size_t);
		void refreshIterators();
		void allocateChunk();
	};


}
