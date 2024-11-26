#include <iostream>
#include <map>
#include "pool_allocator.h"
#include "forward_list.h"

using namespace std;

int factorial(int n)
{
	if (n <= 1)
		return 1;
	return n * factorial(n - 1);
}

int main()
{
	std::map<int, int> basicMap;
	for (int i = 0; i <= 9; i++)
		basicMap[i] = factorial(i);

	std::map<int, int, std::less<int>, PoolAllocator<std::pair<int, int>, 10>> allocatedMap;;
	for (int i = 0; i <= 9; i++)
		allocatedMap[i] = factorial(i);
	for (const auto& kvp : allocatedMap)
		cout << kvp.first << " " << kvp.second << endl;

	ForwardList<int> basicList;
	for (int i = 0; i <= 9; i++)
		basicList.addNew(factorial(i));

 	ForwardList<int, PoolAllocator<int, 10>> allocatedList;
 	for (int i = 0; i <= 9; i++)
 		allocatedList.addNew(factorial(i));
	for (auto i : allocatedList)
		cout << i << " ";
	cout << endl;

	return 0;
}
