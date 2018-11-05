#include <iostream>
#include "MyDeque.h"

using std::cout;
using std::endl;

#define or ||
#define and &&

template<typename Type, typename Val_type>
void _vec_push_front(std::vector<Type> &v, const Val_type &value)
{
	std::reverse(v.begin(), v.end());
	v.push_back(value);
	std::reverse(v.begin(), v.end());
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::show()
{
	if (chunk_map.empty() == true) return;

    typename chunk::iterator cur = chunk_map[0]->begin();
	typename std::vector<chunk*>::iterator mapIt = chunk_map.begin();
	
	while (true)
	{
		if (cur == (*mapIt)->end())
		{
			mapIt++;
			if (mapIt == chunk_map.end()) break;
	
			cur = (*mapIt)->begin();
		}
	
		cout << *cur << endl;
		cur++;
	}
}

template<class T, class Allocator>
 void mystd::deque<T, Allocator>::createMap(size_t n)
{
	 size_t chunkAmount = n / chunk_size + 1;

	 if (n && (n % chunk_size) == 0) --chunkAmount;

	 for (size_t i = 0; i < chunkAmount; i++) allocateChunk();
	 initChunks(n);

	 finish = iterator
	 (
		 chunk_map.back()->end(),
		 chunk_map.end(),
		 chunk_map.end(),
		 chunk_map.begin()
	 );

	 if (n == 0) start = finish;
	 else
	 {
		 start = iterator
		 (
			 chunk_map.front()->begin(),
			 chunk_map.begin(),
			 chunk_map.end(),
			 chunk_map.begin()
		 );
	 }
}

 template<class T, class Allocator>
 void mystd::deque<T, Allocator>::initChunks(size_t n)
 {
	 size_t fullChunks = n / chunk_size;
	 size_t remainder = n - fullChunks * chunk_size;

	 //fill full chunks
	 for (size_t i = 0; i < fullChunks; i++)
		 chunk_map[i]->resize(chunk_size);

	 //fill remainder
	 if (remainder > 0)
		 chunk_map[fullChunks]->resize(remainder);
 }

 template<class T, class Allocator>
 void mystd::deque<T, Allocator>::allocateChunk()
 {
	 chunk *newChunk = chunk_alloc.allocate(1);
	 chunk_alloc.construct(newChunk, data_alloc);

	 chunk_map.push_back(newChunk);
	 chunk_map.back()->reserve(chunk_size);
 }

 template<class T, class Allocator>
mystd::deque<T, Allocator>::deque(size_t n, const Allocator& alloc)
	: data_alloc(alloc)
{
	createMap(n);
}

template<class T, class Allocator>
mystd::deque<T, Allocator>::deque(size_t n, const value_type& init_value, const Allocator& alloc)
	: data_alloc(alloc)
{
	createMap(n);
	std::fill(begin(), end(), init_value);
}

template<class T, class Allocator>
mystd::deque<T, Allocator>::deque
(
	const std::initializer_list<value_type> &_initList, const Allocator& alloc
)
	:data_alloc(alloc)
{
	createMap(_initList.size());
	std::copy(_initList.begin(), _initList.end(), begin());
}

template<class T, class Allocator>
template<class InputIt>
mystd::deque<T, Allocator>::deque(InputIt first, InputIt last, const Allocator & alloc)
	: data_alloc(alloc)
{
	createMap(1);
	std::copy(first, last, std::back_inserter(*this));
}

template<class T, class Allocator>
mystd::deque<T, Allocator>::~deque()
{
	for (chunk *ch : chunk_map)
	{
		chunk_alloc.destroy(ch);
		chunk_alloc.deallocate(ch, 1);
	}

	chunk_map.clear();
	std::vector<chunk*>().swap(chunk_map);
}

template<class T, class Allocator>
size_t mystd::deque<T, Allocator>::size()
{
	if (chunk_map.empty()) return 0;

	size_t summary = 0;
	for (auto ch : chunk_map)
	{
		summary += ch->size();
	}

	return summary;
}


template<class T, class Allocator>
void mystd::deque<T, Allocator>::push_front(const value_type &value)
{
	if (chunk_map.front()->size() == chunk_size)
	{
		std::reverse(chunk_map.begin(), chunk_map.end());
		allocateChunk();
		std::reverse(chunk_map.begin(), chunk_map.end());
	}
	_vec_push_front(*chunk_map.front(), value);
	refreshIterators();
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::push_front(value_type &&value)
{
	push_front(value);
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::push_back(const value_type &value)
{
	if (chunk_map.back()->size() == chunk_size)	 allocateChunk();
	chunk_map[chunk_map.size() - 1]->push_back(value);
	refreshIterators();
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::refreshIterators()
{
	finish = std::move(iterator
	(
		chunk_map.back()->end(),
		chunk_map.end(),
		chunk_map.end(),
		chunk_map.begin()
	));

	start = std::move(iterator
	(
		chunk_map.front()->begin(),
		chunk_map.begin(),
		chunk_map.end(),
		chunk_map.begin()
	));
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::push_back(value_type && x)
{
	push_back(x);
}