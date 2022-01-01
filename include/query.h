
#ifndef QUERY_H
#define QUERY_H

#include <set>
#include "elem.h"
#include "transaction.h"
using namespace std;
namespace TLI{
class Query
{
public:
	Query(){};
	Query(Transaction *tran, int t=3);
	Query(int *d,int len, int t=3);
	~Query(){};

	Transaction * getTrans(){return transaction;}
	Transaction * getsortedTrans(){return sortedQuery;}
	int getT() {return T;}
	void setT(int t) {T=t;}
	void sortQuery(int type,set<Element> *relist,int *remap);
	void setSortedQuery(Transaction *tran){sortedQuery =tran;}
 private:
	Transaction *transaction;
	Transaction *sortedQuery;
	int T;
};
}
#endif QUERY_H
