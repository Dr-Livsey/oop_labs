#include <memory>
#include <vector>

#define or ||
#define and &&

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
		: current(other.current), 
		  mapIterator(other.mapIterator), 
		  mapEnd(other.mapEnd),
		  mapStart(other.mapStart)	{}
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
		  mapEnd(std::move(other.mapEnd)),
		  mapStart(std::move(other.mapStart)) {}

	_RaIt& operator=(const _RaIt&);

	_RaIt& operator++(); //prefix increment
	_RaIt operator++(int); //postfix increment
	_RaIt& operator--(); //prefix decrement
	_RaIt operator--(int); //postfix decrement

	_RaIt& operator-=(int);     
	_RaIt& operator+=(int);  

	_RaIt operator-(int) const;
	_Distance operator-(_RaIt&) const;

	_RaIt operator+(int) const;
	friend _RaIt operator+(int var, const _RaIt &other) { return (*this + var); }
	friend _RaIt operator-(int var, const _RaIt &other) { return (*this - var); }
	friend _Distance operator-(const _RaIt &It1, const _RaIt &It2) { return std::distance(It2, It1); }

	_Reference operator[](int) const;

	_Tp operator*() const { return (*current); }
	_Reference operator*() { return (*current); }
	_Pointer operator->() const { return &(*current); }

	bool operator==(const _RaIt &other) const  
	{ 
		return mapIterator == other.mapIterator && current == other.current; 
	}
	bool operator!=(const _RaIt &other) const { return !(this->operator==(other)); }

	bool operator<(const _RaIt &other) const;
	bool operator>(const _RaIt &other) const;
	bool operator<=(const _RaIt &other) const;
	bool operator>=(const _RaIt &other) const; 

private:
	/*Point to current item in chunk*/
	typename chunk::iterator current;

	/*Point to current chunk*/
	typename std::vector<chunk*>::iterator mapIterator;

	/*End of map*/
	typename std::vector<chunk*>::iterator mapEnd;
	/*End of start*/
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
	current = _Pos;
	mapIterator = _Chunk;
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
	if (mapIterator == mapEnd)
	{
		mapIterator--;
		current--;
		return *this;
	}

	if (current == (*mapIterator)->begin())
	{
		if (mapIterator == mapStart)
		{
			current--;
			return *this;
		}
		else
		{
			mapIterator--;
			current = (*mapIterator)->end() - 1;
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
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator-=(int offset)
{
	return *this += -offset;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> & 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator+=(int offset)
{
	if (mapIterator == mapEnd)
	{
		if (offset > 0)
		{
			/*throw exception*/
			current++;
			return *this;
		}
		else if (offset < 0)
		{
			/*ставим текущий в конец предыдущего chunk*/
			mapIterator--;
			current = (*mapIterator)->end() - 1;
			offset++;
		}
	}

	/*сдвиг относительно начала текущего chunk*/
	offset = (current - (*mapIterator)->begin()) + offset;
	current = (*mapIterator)->begin();

	while (offset)
	{
		if (offset > 0)
		{
			if (mapIterator == mapEnd) return *this;
			else
			{
				int chunk_size = (*mapIterator)->end() - (*mapIterator)->begin();

				if (offset >= chunk_size)
				{
					mapIterator++;
					if (mapIterator != mapEnd) 
						current = (*mapIterator)->begin();
					offset -= chunk_size;
				}
				else
				{
					current += offset;
					offset = 0;
				}
			}
		}
		else
		{
			/*текущее положение итератора current в chunk*/
			int curPos = current - (*mapIterator)->begin();
			/*размер текущего chunk*/
			int chunk_size = (*mapIterator)->end() - (*mapIterator)->begin();
			bool more = (curPos + offset) < 0;

			if (more)
			{
				mapIterator--;
				current = (*mapIterator)->end() - 1;
				offset += curPos + 1;
			}
			else
			{
				current += offset;
				offset = 0;
			}
		}
	}

	return *this;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator> 
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator-(int dec_val) const
{
	_RaIt oldIt = *this;
	return oldIt -= dec_val;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>
	random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator+(int inc_val) const
{
	_RaIt oldIt = *this;
	return oldIt += inc_val;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
_Reference random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator[](int n) const
{
	return *(*this + n);
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
bool random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator<(const _RaIt & other) const
{
	if (mapIterator == other.mapIterator)
	{
		return current < other.current;
	}
	return mapIterator < other.mapIterator;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
bool random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator>(const _RaIt & other) const
{
	if (mapIterator == other.mapIterator)
	{
		return current > other.current;
	}
	return mapIterator > other.mapIterator;
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
bool random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator<=(const _RaIt & other) const
{
	return !(*this > other);
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
bool random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator>=(const _RaIt & other) const
{
	return !(*this < other);
}

template<typename _Tp, typename _Distance, typename _Pointer, typename _Reference, typename _Allocator>
_Distance random_access_iterator<_Tp, _Distance, _Pointer, _Reference, _Allocator>::operator-(_RaIt &_other) const
{	
	_RaIt oldIt = _other;
	_RaIt other = *this;
	bool is_swapped = false;

	/*oldIt must be less then other*/
	if (!(oldIt < other))
	{
		_RaIt temp(oldIt);
		oldIt = other;
		other = temp;
		is_swapped = true;
	}

	if (oldIt.mapIterator == other.mapIterator)
	{
		return other.current - oldIt.current;
	}
	else
	{
		_Distance offset = 0;

		/*skip chunks*/
		while (oldIt.mapIterator != other.mapIterator)
		{

			offset += (*oldIt.mapIterator)->end() - oldIt.current;

			oldIt.mapIterator++;
			if (oldIt.mapIterator != mapEnd)
				oldIt.current = (*oldIt.mapIterator)->begin();
			else
			{
				return is_swapped ? -offset :  offset;
			}
		}

		offset += (other.current - oldIt.current);

		return is_swapped ? -offset : offset;
	}
}