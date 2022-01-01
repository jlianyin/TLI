#include "include/query.h"
#include <vector>
#include <algorithm>

#include "include/util.h"

using namespace std;
using namespace TLI;
Query::Query(Transaction *tran, int t)
{
	transaction = tran;
	T=t;
}

Query::Query(int *d,int len, int t)
{
	transaction =new  Transaction(len);
	for(int i=0;i<len;i++)
	{
		transaction->t[i] = d[i];
	}
	/*transaction->t = d;*/
	T=t;
}

void Query::sortQuery(int type,set<Element> *relist,int *remap)
{
	Transaction * t= getTrans();
	sortedQuery = new Transaction(t->length);
	//lex order
	if(type==LEX_ORDER)
	{

		/*heap_sort(t->t,t->length);*/
		for(int i=0;i<t->length;i++)
		{
			sortedQuery->t[i] = t->t[i];
		}
	}
	else if(type ==WORDFRE_ORDER)//word frequecy order
	{
		vector<int> list;
		int i=0;

		for(i =0;i<t->length;i++)
		{
			set<Element>::iterator it = relist->find(Element(t->t[i],0));
			if(it != relist->end()) list.push_back(it->id);
		}
		sort(list.begin(),list.end());

		for(i=0;i<t->length;i++)
		{
			sortedQuery->t[i] = t->t[i];
			t->t[i] = remap[list[i]];
		}
	}
}
