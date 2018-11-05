#include <iostream>
#include <deque>
#include <algorithm>

#include "MyDeque.h"
#include "MyDeque.cpp"

using std::cout;
using std::endl;

/*
	1. when init with initlist first element is 0

*/

int main()
{	
	mystd::deque<int> mydeq = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	mystd::deque<int>::iterator It;

	It = mydeq.begin();
	It += 17;

	cout << *It  << endl;
	


	//for (auto It = mydeq.end(); It != mydeq.begin(); --It)
	//{
	//	cout << *It << endl;
	//}


	//for (auto It = mydeq.begin(); It != mydeq.end(); It++)
	//{
	//	cout << *It << endl;
	//}

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
