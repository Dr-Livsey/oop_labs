#include <memory>
#include <vector>

template
<
	typename _Tp,
	typename _Distance = ptrdiff_t,
	typename _Pointer = _Tp * ,
	typename _Reference = _Tp &,
	typename _Allocator = std::allocator<_Tp>
>
class random_access_iterator :
	public std::iterator<std::random_access_iterator_tag, _Tp, _Distance, _Pointer, _Reference>
{
	typedef random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> _RaIt;
	typedef std::vector<_Tp, _Allocator> chunk;
public:
	random_access_iterator() {}
	random_access_iterator(const _RaIt &other) 
		: current(other.current), mapIterator(other.mapIterator), mapEnd(other.mapEnd) {}
	random_access_iterator
	(
		const typename chunk::iterator&, 
		const typename std::vector<chunk*>::iterator&,
		const typename std::vector<chunk*>::iterator&,
		const typename std::vector<chunk*>::iterator&
	);
	random_access_iterator(_RaIt &&other)
		: current(std::move(other.current)), 
			mapIterator(std::move(other.mapIterator)), 
				mapEnd(std::move(other.mapEnd)) {}

	_RaIt& operator=(const _RaIt&);

	_RaIt& operator++(); //prefix increment
	_RaIt operator++(int); //postfix increment

	_RaIt& operator--(); //prefix decrement
	_RaIt operator--(int); //postfix decrement
	_RaIt& operator-=(int); /*Not implemented*/     
	_RaIt& operator+=(int); /*Not implemented*/     
	_RaIt operator-(int) const; /*Not implemented*/

	_Tp operator*() const { return (*current); }
	_Reference operator*() { return (*current); }
	_Pointer operator->() const { return &(*current); }

	bool operator==(const _RaIt &other) const  
	{ 
		return mapIterator == other.mapIterator && current == other.current; 
	}
	bool operator!=(const _RaIt &other) const { return !(this->operator==(other)); }

private:
	/*Point to current item in vector*/
	typename chunk::iterator current;

	/*Point to current chunk*/
	typename std::vector<chunk*>::iterator mapIterator;

	/*End of map*/
	typename std::vector<chunk*>::iterator mapEnd;
	/*End of map*/
	typename std::vector<chunk*>::iterator mapStart;
};


template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::random_access_iterator
(
	const typename chunk::iterator &_Pos, 
	const typename std::vector<chunk*>::iterator &_Chunk,
    const typename std::vector<chunk*>::iterator &_mapEnd,
	const typename std::vector<chunk*>::iterator &_mapStart)
{
	mapIterator = _Chunk;
	current = _Pos;
	mapEnd = _mapEnd;
	mapStart = _mapStart;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>&
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator=
(
	const _RaIt &other
)
{
	current = other.current;
	mapIterator = other.mapIterator;
	mapEnd = other.mapEnd;
	mapStart = other.mapStart;

	return *this;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>&
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator++()
{
	if (mapIterator == mapEnd) return *this;

	current++;

	if (current == (*mapIterator)->end())
	{
		mapIterator++;
		if (mapIterator != mapEnd)	
			current = (*mapIterator)->begin();
	}

	return *this;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator++(int)
{
	_RaIt oldIt = *this;
	++*this;
	return oldIt;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> & 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator--()
{
	if (current == (*mapIterator)->begin())
	{
		if (mapIterator == mapStart) return *this;
		else
		{
			mapIterator--;
			current = (*mapIterator)->end();
		}
	}
	else
	{
		current--;
	}

	return *this;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator--(int)
{
	_RaIt oldIt = *this;
	--*this;
	return oldIt;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>& 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator-=(int)
{
	/*Not implemented*/
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> & 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator+=(int offset)
{
	if (offset >= 0)
	{
		if (mapEnd == mapIterator) 
			return *this;
		else if (std::next(mapIterator) == mapEnd)
		{
			current += offset;
		}
		else
		{
			int chunk_size = (*mapIterator)->end() - (*mapIterator)->begin();
			int full_chunks = offset / chunk_size;

			mapIterator += full_chunks;

			if (mapEnd != mapIterator)
				current = (*mapIterator)->begin() + (offset % chunk_size);
			else
				current = (*(mapIterator - 1))->end();
		}
	}
	else
	{
		/*Not implemented*/
	}


	return *this;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator-(int dec_val) const
{
	/*Not implemented*/
}