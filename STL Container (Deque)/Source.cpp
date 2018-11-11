#include <iostream>
#include <deque>
#include <string>
#include <algorithm>

#include "MyDeque.h"
#include "MyDeque.cpp"

using std::cout;
using std::endl;

/*
	1. when init with initlist first element is 0 *FIXED*
	2. std::sort is not effect *FIXED*
	3. after line 23 iterators was incorrect (mapIterator) *FIXED*
*/

int main()
{
	//mystd::deque<int> mydeq = { 10,2,4, 11,13 };
	mystd::deque<int> mydeq = { 1,2,3,4,5,6,7,8, 10 };
	//std::deque<std::string> mydeq;

	
	for (auto It = mydeq.begin(); It != mydeq.end(); It++)
	{
		cout << *It << endl;
	}



	system("pause");
	return 0;
}

//std::vector<int> *v;
//std::allocator<std::vector<int>> chunkalloc;
//std::allocator<std::string> intalloc;


//v = chunkalloc.allocate(1);
//chunkalloc.construct(v, intalloc);

//v->push_back(1);

//chunkalloc.destroy(v);
//chunkalloc.deallocate(v, 1);
