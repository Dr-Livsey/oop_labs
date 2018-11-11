#pragma once
#include <memory>
#include <vector>

#define or ||
#define and &&

namespace bit
{
	template <class Container>
	class back_insert_iterator
		: public std::iterator<std::output_iterator_tag, void, void, void, void>
	{
	public:
		using container_type =			typename Container;
		using iterator_category =		typename std::output_iterator_tag;
		using reference =				void;
		using pointer =					void;
		using difference_type =			void;
		using value_type =				void;

		/*Constructors*/
		explicit						 back_insert_iterator(Container& c) : cont(&c) {}
										 back_insert_iterator() : cont(0) {}
										 ~back_insert_iterator() {}

		/*Operators*/
		back_insert_iterator<Container>& operator=(typename Container::const_reference value);
		back_insert_iterator<Container>& operator=(typename Container::value_type&& value);

		/*Provided to satisfy the requirements of OutputIterator.*/
		back_insert_iterator&			 operator*() { return *this; }
		back_insert_iterator&			 operator++() { return *this; }
		back_insert_iterator&			 operator++(int) { return *this; }

	protected:
		Container *cont;
	};

}

template<class Container>
bit::back_insert_iterator<Container>& bit::back_insert_iterator<Container>::operator=
(
	typename Container::const_reference value
)
{
	cont->push_back(value);
	return *this;
}

template<class Container>
bit::back_insert_iterator<Container>& bit::back_insert_iterator<Container>::operator=
(
	typename Container::value_type&& value
)
{
	cont->push_back(std::move(value));
	return *this;
}