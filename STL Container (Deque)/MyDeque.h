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

		using reference = T & ;
		using const_reference = const T&;

		using difference_type = typename std::allocator_traits<Allocator>::difference_type;
		using size_type = typename std::allocator_traits<Allocator>::size_type;

		using iterator = typename random_access_iterator<T, difference_type, pointer, reference, Allocator>;
		using reverse_iterator = typename std::reverse_iterator<iterator>;

		using iterator_category = std::random_access_iterator_tag;

		deque(const deque<T, Allocator>& other);
		deque(const deque<T, Allocator>& other, const Allocator& alloc);
		deque(deque<T, Allocator>&& other);
		deque(deque<T, Allocator>&& other, const Allocator& alloc);

		deque(size_t n = 0, const Allocator& alloc = Allocator());
		deque(size_t, const value_type&, const Allocator& alloc = Allocator());
		deque(const std::initializer_list<value_type>&, const Allocator& alloc = Allocator());
		template<class InputIt> deque(InputIt first, InputIt last, const Allocator& alloc = Allocator());

		~deque();

		void				assign(size_type count, const_reference value);
		template<class InputIt>
		void				assign(InputIt first, InputIt last);
		void				assign(std::initializer_list<T> ilist);

		void				swap(deque& other);

		size_type			size();
		void				resize(size_type count, value_type value = value_type());
		size_type			max_size() const { return std::numeric_limits<size_type>::max(); }

		bool				empty() const { return start == finish; }
		void				clear();

		reverse_iterator	rbegin() { return reverse_iterator(finish); }
		reverse_iterator	rend() { return reverse_iterator(start); }
		iterator			begin() { return start; }
		iterator			end() { return finish; }

		reference			front() { return (*start); }
		const_reference		front() const { return const_cast<const_reference>((*start)); }
		reference			back() { return (*(finish - 1)); }
		const_reference		back() const { return const_cast<const_reference>((*(finish - 1))); }

		iterator			erase(iterator pos);
		iterator			erase(iterator first, iterator last);

		iterator			insert(iterator pos, const_reference value);
		iterator			insert(iterator pos, T&& value);
		iterator			insert(iterator pos, size_type count, const_reference value);
		iterator			insert(iterator pos, std::initializer_list<T> ilist);
		template<class InputIt>
		iterator			insert(iterator pos, InputIt first, InputIt last);

		template<class... Args>
		iterator			emplace(iterator pos, Args&&... args);
		template<class... Args>
		void				emplace_back(Args&&... args);
		template<class... Args>
		void				emplace_front(Args&&... args);

		void				push_front(const value_type&);
		void				push_front(value_type&&);
		void				push_back(const value_type&);
		void				push_back(value_type&&);

		void				pop_back() { erase(finish - 1); }
		void				pop_front() { erase(start); }

		friend bool			operator==(const deque&lhs, const deque&rhs)
		{
			size_type s = lhs.chunk_map.size();

			if (s == rhs.chunk_map.size())
			{
				for (size_type i = 0; i < s; i++)
				{
					if ((*lhs.chunk_map[i]) != (*rhs.chunk_map[i]))
					{
						return false;
					}
				}
				return true;
			}
			else return false;
		}
		friend bool			operator!=(const deque& lhs, const deque& rhs) { return !(lhs == rhs); }
		friend bool			operator<(deque& lhs, deque& rhs)
		{
			 return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
		}
		friend bool			operator<=(deque& lhs, deque& rhs)
		{
			return !(lhs > rhs);
		}
		friend bool			operator>(deque& lhs, deque& rhs)
		{
			return std::lexicographical_compare
			(
				lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::greater<value_type>()
			);
		}
		friend bool			operator>=(deque& lhs, deque& rhs)
		{
			return !(lhs < rhs);
		}

		allocator_type		get_allocator() const { return data_alloc; };

		/*Operators*/
		deque<T, Allocator>& operator=(const deque<T, Allocator>& other);
		deque<T, Allocator>& operator=(deque<T, Allocator>&& other); /*Not implemented*/

		reference		operator[](size_type pos) { return (*(start + pos)); }
		reference		at(size_type pos)
		{
			if (!(pos < this->size()))
				throw std::out_of_range("Position is out of range");

			return this->operator[](pos);
		}
		const_reference operator[](size_type pos) const 
		{
			return const_cast<const_reference>(*(start + pos)); 
		}
	
	//	using const_iterator
	//	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	private:
		std::vector<chunk*> chunk_map;
		size_t chunk_size = 8;

		chunkAllocator chunk_alloc;
		Allocator data_alloc;

		iterator start;
		iterator finish;

		void refreshIterators();

		void createMap(size_t);
		void initChunks(size_t);
		void allocateChunk();

		void construct(const deque<T, allocator_type>&, const Allocator&);
		void clean_up();

		void free_chunk(typename std::vector<chunk*>::iterator&);
		typename std::vector<chunk*>::iterator get_chunk(size_type &idx);
	};


}