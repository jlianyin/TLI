#ifndef NODEARRAY_H
#define NODEARRAY_H
#include <vector>

using namespace std;
namespace TLI{
class NodeArray
{
public:
	NodeArray(){arr=new vector<int>;}
	NodeArray(int l){arr=new vector<int>;}
	~NodeArray(){ delete []arr;}

	vector<int> *getArray() {return arr;}

private:
	vector<int> *arr;
};
}
#endif NODEARRAY_H
