#include <iostream>
#include <iterator>
#include "MyDeque.h"

using std::cout;
using std::endl;

#define or ||
#define and &&

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
 void mystd::deque<T, Allocator>::construct(const deque<T, allocator_type> &_other, const Allocator &alloc)
 {
	 deque<T, Allocator> &other = const_cast<deque<T, Allocator>&>(_other);

	 data_alloc = other.data_alloc;

	 createMap(0);
	 std::copy(other.begin(), other.end(), std::back_inserter(*this));
 }

 template<class T, class Allocator>
 void mystd::deque<T, Allocator>::clean_up()
 {
	 for (chunk *ch : chunk_map)
	 {
		 if (ch != nullptr)
		 {
			 chunk_alloc.destroy(ch);
			 chunk_alloc.deallocate(ch, 1);
		 }
	 }

	 chunk_map.clear();
	 std::vector<chunk*>().swap(chunk_map);
 }

 template<class T, class Allocator>
 void mystd::deque<T, Allocator>::free_chunk(typename std::vector<chunk*>::iterator &ch)
 {
	 bool is_start = (ch == chunk_map.begin());

	 chunk_alloc.destroy(*ch);
	 chunk_alloc.deallocate(*ch, 1);

	 chunk_map.erase(ch);
	 chunk_map.shrink_to_fit();

	 if (is_start) refreshIterators();
 }

 template<class T, class Allocator>
 typename std::vector<std::vector<T, Allocator>*>::iterator 
	 mystd::deque<T, Allocator>::get_chunk
 (
	 typename mystd::deque<T, Allocator>::size_type &idx
 )
 {
	 for (auto chunkIt = chunk_map.begin(); chunkIt <= chunk_map.end(); chunkIt++)
	 {
		 size_type cSize = (chunkIt == chunk_map.end() ? 1 : (*chunkIt)->size());
		 
		 if (idx < cSize) return chunkIt;
		 else
		 {
			 idx -= cSize;
		 }
	 }
	 
	 return chunk_map.end();
 }

 template<class T, class Allocator>
 mystd::deque<T, Allocator>::deque(const deque<T, Allocator>& _other)
 {
	 construct
	 (
		 _other, 
		 std::allocator_traits<allocator_type>::select_on_container_copy_construction(_other.data_alloc)
	 );
 }

 template<class T, class Allocator>
 mystd::deque<T, Allocator>::deque(const deque<T, Allocator> &other, const Allocator &alloc)
	 : data_alloc(alloc)
 {
	 construct(other, alloc);
 }

 template<class T, class Allocator>
 mystd::deque<T, Allocator>::deque(deque<T, Allocator>&& other)
 {
	 construct(other, std::move(other.data_alloc));
 }
 template<class T, class Allocator>
 mystd::deque<T, Allocator>::deque(deque<T, Allocator>&& other, const Allocator & alloc)
 {
	 construct(other, alloc);
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
	createMap(0);
	std::copy(first, last, std::back_inserter(*this));
}

template<class T, class Allocator>
mystd::deque<T, Allocator>::~deque()
{
	clean_up();
}

template<class T, class Allocator>
typename mystd::deque<T, Allocator>::size_type mystd::deque<T, Allocator>::size()
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
		refreshIterators();
	}
	insert(start, value);
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::push_front(value_type &&value)
{
	if (chunk_map.front()->size() == chunk_size)
	{
		std::reverse(chunk_map.begin(), chunk_map.end());
		allocateChunk();
		std::reverse(chunk_map.begin(), chunk_map.end());
		refreshIterators();
	}

	std::reverse(chunk_map[0]->begin(), chunk_map[0]->end());
	chunk_map[0]->push_back(std::forward<T>(value));
	std::reverse(chunk_map[0]->begin(), chunk_map[0]->end());

	refreshIterators();
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

	if (!size())
	{
		start = finish;
		return;
	}

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
	if (chunk_map.back()->size() == chunk_size)	 allocateChunk();
	chunk_map[chunk_map.size() - 1]->push_back(std::forward<T>(x));
	refreshIterators();
}

template<class T, class Allocator>
mystd::deque<T, Allocator>& mystd::deque<T, Allocator>::operator=(const deque<T, Allocator>& other)
{
	clean_up();
	construct(other, other.data_alloc);
	return *this;
}

template<class T, class Allocator>
mystd::deque<T, Allocator>& mystd::deque<T, Allocator>::operator=(deque<T, Allocator>&& other)
{
	clean_up();
	std::vector<chunk*> &_chunk_map = other.chunk_map;

	chunk_map.resize(_chunk_map.size());

	for (size_t i = 0; i < _chunk_map.size(); i++)
	{
		chunk_map[i] = _chunk_map[i];
		_chunk_map[i] = nullptr;
	}

	refreshIterators();
	return *this;
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::clear(void)
{
	clean_up();
	createMap(0);
}

template<class T, class Allocator>
typename mystd::deque<T, Allocator>::iterator mystd::deque<T, Allocator>::erase
(
	typename mystd::deque<T, Allocator>::iterator pos
)
{
	size_type idx = pos - start, last_elemIdx;
	bool isEnd = false;
	typename std::vector<chunk*>::iterator sel_chunk = get_chunk(idx);

	typename chunk::iterator last_pos = (*sel_chunk)->erase((*sel_chunk)->begin() + idx);

	if (last_pos == (*sel_chunk)->end())
	{
		last_elemIdx = sel_chunk - chunk_map.begin() + 1;

		if (sel_chunk + 1 != chunk_map.end())
		{
			last_pos = (*(sel_chunk + 1))->begin();
		}
		else isEnd = true;
	}
	else last_elemIdx = sel_chunk - chunk_map.begin();

	/*this chunk is empty. must free*/
	if ((*sel_chunk)->empty() && chunk_map.size() != 1)
	{
		free_chunk(sel_chunk);
		last_elemIdx--;
	}

	refreshIterators();

	typename std::vector<chunk*>::iterator mapIt = chunk_map.begin() + last_elemIdx;
	typename chunk::iterator curIt = (isEnd ? (*(mapIt - 1))->end() : last_pos);

	iterator retval
	(
		curIt,
		mapIt,
		chunk_map.end(), 
		chunk_map.begin()
	);

	return retval;
}

template<class T, class Allocator>
typename mystd::deque<T, Allocator>::iterator mystd::deque<T, Allocator>::erase
(
	typename mystd::deque<T, Allocator>::iterator first, 
	typename mystd::deque<T, Allocator>::iterator last
)
{
	size_type interval = last - first;

	while (interval > 0)
	{
		first = erase(first);
		interval--;
	}

	return first;
}

template<class T, class Allocator>
typename mystd::deque<T, Allocator>::iterator
mystd::deque<T, Allocator>::insert
(
	typename mystd::deque<T, Allocator>::iterator pos,
	typename mystd::deque<T, Allocator>::const_reference value
)
{
	size_type idx = pos - start;
	typename std::vector<chunk*>::iterator ins_chunk = get_chunk(idx);

	if (!idx && ins_chunk == chunk_map.end())
	{
		ins_chunk--;
		idx = (*ins_chunk)->size();
	}

	/*idx is changed. old idx = new idx + ins_chunk->begin()*/
	typename chunk::iterator ins_it = (*ins_chunk)->insert((*ins_chunk)->begin() + idx, value);

	if (ins_chunk == chunk_map.begin())
		refreshIterators();
	else if (ins_chunk + 1 == chunk_map.end())
		refreshIterators();

	iterator retval(ins_it, ins_chunk, chunk_map.end(), chunk_map.begin());
	return retval;
}

template<class T, class Allocator>
typename mystd::deque<T, Allocator>::iterator
mystd::deque<T, Allocator>::insert
(
	typename mystd::deque<T, Allocator>::iterator pos, 
	T && value
)
{
	return insert(pos, value);
}

template<class T, class Allocator>
typename mystd::deque<T, Allocator>::iterator 
mystd::deque<T, Allocator>::insert
(
	typename mystd::deque<T, Allocator>::iterator pos,
	typename mystd::deque<T, Allocator>::size_type count,
	typename mystd::deque<T, Allocator>::const_reference value
)
{
	if (count == 0) return pos;

	size_type idx = pos - start;
	bool isEmpty = empty();

	std::fill_n(std::inserter(*this, pos), count, value);
	refreshIterators();

	return (isEmpty ? start : (start + idx));
}
	
template<class T, class Allocator>
typename mystd::deque<T, Allocator>::iterator
mystd::deque<T, Allocator>::insert
(
	typename mystd::deque<T, Allocator>::iterator pos,
	std::initializer_list<T> init_list
)
{
	if (!init_list.size()) return pos;

	size_type idx = pos - start;
	bool isEmpty = empty();

	std::copy(init_list.begin(), init_list.end(), std::inserter(*this, pos));
	refreshIterators();

	return (isEmpty ? start : (start + idx));
}

template<class T, class Allocator>
template<class InputIt>
typename mystd::deque<T, Allocator>::iterator 
mystd::deque<T, Allocator>::insert
(
	typename mystd::deque<T, Allocator>::iterator pos, 
	InputIt first, 
	InputIt last
)
{
	if (first == last) return pos;

	size_type idx = pos - start;
	bool isEmpty = empty();

	std::copy(first, last, std::inserter(*this, pos));
	refreshIterators();

	return (isEmpty ? start : (start + idx));
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::assign
(
	typename mystd::deque<T, Allocator>::size_type count, 
	typename mystd::deque<T, Allocator>::const_reference value
)
{
	clean_up();
	createMap(0);
	insert(start, count, value);
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::assign(std::initializer_list<T> ilist)
{
	clean_up();
	createMap(0);
	insert(start, ilist);
}

template<class T, class Allocator>
template<class InputIt>
void mystd::deque<T, Allocator>::assign(InputIt first, InputIt last)
{
	clean_up();
	createMap(0);
	insert(start, first, last);
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::swap(deque<T, Allocator> &right)
{
	chunk_map.swap(right.chunk_map);
	std::swap(start, right.start);
	std::swap(finish, right.finish);
}

template<class T, class Allocator>
void mystd::deque<T, Allocator>::resize(size_type count, value_type value)
{
	size_type c_size = size();

	if (c_size > count)
	{
		size_type diff = c_size - count;
		this->erase(finish - diff, finish);
	}
	else if (c_size < count)
	{
		this->insert(finish, count - c_size, value);
	}
}

template<class T, class Allocator>
template<class ...Args>
typename mystd::deque<T, Allocator>::iterator mystd::deque<T, Allocator>::emplace
(
	iterator pos, Args && ...args
)
{
	size_type idx = pos - start;
	typename std::vector<chunk*>::iterator ins_chunk = get_chunk(idx);

	if (!idx && ins_chunk == chunk_map.end())
	{
		ins_chunk--;
		idx = (*ins_chunk)->size();
	}

	/*idx is changed. old idx = new idx + ins_chunk->begin()*/
	typename chunk::iterator ins_it = (*ins_chunk)->emplace
	(
		(*ins_chunk)->begin() + idx, std::forward<T>(args)...
	);

	if (ins_chunk == chunk_map.begin())
		refreshIterators();
	else if (ins_chunk + 1 == chunk_map.end())
		refreshIterators();

	iterator retval(ins_it, ins_chunk, chunk_map.end(), chunk_map.begin());
	return retval;
}

template<class T, class Allocator>
template<class ...Args>
void mystd::deque<T, Allocator>::emplace_back(Args && ...args)
{
	if (chunk_map.back()->size() == chunk_size)	 allocateChunk();
	chunk_map[chunk_map.size() - 1]->emplace_back(std::forward<T>(args)...);
	refreshIterators();
}

template<class T, class Allocator>
template<class ...Args>
void mystd::deque<T, Allocator>::emplace_front(Args && ...args)
{
	if (chunk_map.front()->size() == chunk_size)
	{
		std::reverse(chunk_map.begin(), chunk_map.end());
		allocateChunk();
		std::reverse(chunk_map.begin(), chunk_map.end());
		refreshIterators();
	}

	std::reverse(chunk_map[0]->begin(), chunk_map[0]->end());
	chunk_map[0]->emplace_back(std::forward<T>(args)...);
	std::reverse(chunk_map[0]->begin(), chunk_map[0]->end());

	refreshIterators();
}