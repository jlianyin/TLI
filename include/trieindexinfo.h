/*----------------------------------------------------------------------
  File    : fpgrowth.h
  Contents: fpgrowth algorithm for finding frequent sets
  Author  : Bart Goethals
  Update  : 4/4/2003
  ----------------------------------------------------------------------*/
#ifndef TRIXINDEXINFO_H
#define TRIXINDEXINFO_H
#include "query.h"
#include "util.h"
#include <map>
#include <vector>
#include "Record.hpp"
//#include "louds_sparse.hpp"
using namespace std;
namespace surf {

class TrieIndexInfo
{
 public:

  TrieIndexInfo();
  ~TrieIndexInfo();

  void setData(Data *d){data = d;}
  void setMinsup(unsigned ms){minsup = ms;}
  void setOutput(char *of);
  void setElementOrder(ElementOrder order) {elemorder =order; }
  void setListWhere(ListWhere pos) {listwhere =pos; trie->setListWhere(pos);}
  void setQuery(Query *qry) {query = qry;}
 /* int mine();*/
  void createIndex(int cutofflevel);
  void computUpper();
 //int lookupKey(const Record& key) ;
  //void writeIndex();

  //void readIndex();

  void writeCreateInfo();

  vector<int> search(Transaction *query,int T);

  //vector<int> searchMapWithLenPrune(Transaction *query,int T);

  //vector<int> searchWithLenPrune(Transaction *query,int T);

 // vector<int> searchExactWithLenPrune(Transaction *query,int T);

  //vector<int> searchExactSimilarity(Transaction *query,float simthres,SimilarityType type);

  //vector<int> traverseWithLenPrune(Transaction *query,int T);

  //vector<int> probeWithLenPrune(Transaction *query,int T);

  //vector<int> scanCountWithLenPrune(Transaction *query,int T);

  ///vector<int> mergeSkipWithLenPrune(Transaction *query,int T);

  //vector<int> subsetSearch(Transaction *query);
  //vector<int> equalitySearch(Transaction *query);
  vector<int> upperSearch(vector<int>& b,Transaction *query,int cutofflevel);


 // map<int,vector<int>> join(vector<Transaction *> querys,int T);

 // map<int,vector<int>> joinWithLengthPrune(vector<Transaction *> querys,int T);

  //void transformQuerys(vector<Transaction *> querys);

  //void transformQuery(Transaction *query);
  //void transformQueryWithSort(Transaction *query);

  Trie *getTrie(){return trie;}


  //void generateQuery();

  void destory();

 private:

  unsigned minsup;
  Data *data;
  Query *query;
  Trie *trie;
  FILE *out;
  ElementOrder elemorder;
  ListWhere listwhere;


public:
	char *dirfile,*listfile,*nodefile,*statfile;
	 FILE *stat,*dir,*list,*node;

	int transcount;
	int totalelemcount;
	int maxtranslen,mintranslen;
	int maxlistlen,minlistlen;
	int *translen;
	int simfound;
	int candidate;
	int spacemem;
};

}
#endif TRIXINDEXINFO_H
