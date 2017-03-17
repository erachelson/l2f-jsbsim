#include <vector>
#include <iostream>

using namespace std;

int main()
{
	vector<int> haha;
	vector<int>::iterator it;
	it = haha.begin();
	haha.push_back(1);
	haha.push_back(2);
	haha.push_back(3);
	for (auto ha:haha)
	{
		cout<<ha<<endl;
	}
	//haha.insert(it,2,5);
	haha.erase(haha.begin());
	haha.push_back(100);
	for (auto ha:haha)
	{
		cout<<ha<<"  #"<<endl;
	}
	return(0);
}