#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include <set>
#include <vector>
#include "query.h"
#include "elem.h"
#include "transaction.h"
using namespace std;

namespace TLI{
class DataInfo
{
public:
	DataInfo(){};
	DataInfo(int count,int totalcount,int max,int min,set<int> terms);
	~DataInfo(){};
	set<int> getDistinctTerms(){return distinctTerms;}

public:
	int transcount;
	int totalelemcount;
	int minlen;
	int maxlen;
	set<int> distinctTerms;
};

class Data
{
 public:

  Data(char *filename, int t=0);
  Data(char *datafilename,char *distinctfilename,char *rawfilename, int t=0);
  ~Data();

  Transaction *trans;
  DataInfo *info;

  Transaction *getNext();
  Transaction *getNext1();
  void readData();
  void readQuery(vector<Query*> *querys,char *queryfile);
  void readQuery(vector<Transaction *> *trans,char *queryfile,int includeID);
  void readQueryBinary(vector<Transaction *> *trans,char *queryfile,int includeID);
  Transaction *getNextAsFlat(FILE *fp);

  Transaction *getNextAsFlatWithID(FILE *fp);


  void sortData(int type,set<Element> *relist,int *remap);
  //void sortQuery(int type,set<Element> *relist,int *remap);
  int readMswebSingleLine(FILE *rawin,int *flag);
  void getMswebData(FILE *rawin,FILE *out=NULL);
  void getDataInfo();
  //void generatQuery(int querycount);
  vector<Transaction *>  *generatQuery(int querycount,char *queryfile);

  void addTranToQuery(Transaction * t,vector<Transaction *> *querys,int len,int rules);

  void selectTrans(vector<Transaction *> *querys,int count,int minlen,int qualifiednum,int rules);

  void mulitpleTrans(vector<Transaction *> *querys,int countnow,int countfinal);

  void generatQueryThreshold(vector<Transaction *> *querys,int querycount,char *queryfile,int *threshold,int count);

  void generatQueryOK(vector<Transaction *> *querys,int querycount,char *queryfile,int *cardinality,int count,int srules,int trules,int includeID);


  void generateDuplicate(double ratio,char *outfile);

  void reverseQuery(vector<Transaction *> *querys,char *remapfile,char *newqueryfile,int includeID);
private:
	Transaction *getNextAs();
	Transaction *getNextAsFlat();
	Transaction *getNextAsQuest();
	Transaction *getNextBin();


  FILE *in;
  char *datafile;
  char *distinctfile;
  char *rawfile;
  int current;
  int type;
  int transcount;
};
}

#endif DATA_H
