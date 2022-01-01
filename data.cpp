#include "include/data.h"
#include <algorithm>
#include "include/util.h"
#include <stdio.h>
#include <string>

using namespace std;
using namespace TLI;
Transaction::Transaction(const Transaction &tr)
{
	length = tr.length;
	t = new int[tr.length];
	for(int i=0; i< length; i++) t[i] = tr.t[i];
}

Data::Data(char *filename, int t)
{
	datafile = filename;
	type = t;
	current=0;
	transcount=0;

}
 Data::Data(char *datafilename,char *distinctfilename,char *rawfilename, int t)
 {
	 datafile = datafilename;
	 distinctfile = distinctfilename;
	 rawfile= rawfilename;
	 type=t;
	 current=0;
	 transcount=0;

 }
 DataInfo::DataInfo(int count,int totalcount,int max,int min,set<int> terms)
 {
	 transcount=count;
	 totalelemcount =totalcount;
	 maxlen=max;
	 minlen = min;
	 distinctTerms=terms;
 }

Data::~Data()
{
	if(in!=NULL) fclose(in);
}

void Data::readQuery(vector<Transaction *> *trans,char *queryfile,int includeID)
{

	FILE *fp=fopen(queryfile,"r");

	if(includeID == 1)
	{
		//Transaction *pre = getNextAsFlat(fp);
		Transaction *pre = getNextAsFlatWithID(fp);

		trans->push_back(pre);
		//while(Transaction *t = getNextAsFlat()) {
		while(Transaction *t = getNextAsFlatWithID(fp)) {
			trans->push_back(t);
		}
	}
	else
	{
		Transaction *pre = getNextAsFlat(fp);
		//Transaction *pre = getNextAsFlatWithID(fp);

		trans->push_back(pre);
		while(Transaction *t = getNextAsFlat())
		{
		//while(Transaction *t = getNextAsFlatWithID(fp)) {
			trans->push_back(t);
		}
	}
	fclose(fp);
}

void Data::readQueryBinary(vector<Transaction *> *trans,char *queryfile,int includeID)
{

	FILE *fp=fopen(queryfile,"rb");

	int queryid;
	int len;
	int i=0;

	if(includeID == 1)
	{

		while (( fread(&queryid, 1, 4, fp)) == 4)
		{

			fread(&len,sizeof(int),1,fp);
			int *data = new int[len];
			fread(data,sizeof(int),len,fp);

			Transaction *t = new Transaction(len);
			t->id = queryid;
			for(int i =0;i<len;i++)
			{
				t->t[i] = data[i];
			}
			trans->push_back(t);
			delete data;
		}
	}

	fclose(fp);
}


void Data::readQuery(vector<Query *> *querys,char *queryfile)
{

	FILE *fp=fopen(queryfile,"r");

	Transaction *pre = getNextAsFlat(fp);
	Query *query = new Query(pre);
	querys->push_back(query);
	while(Transaction *t = getNextAsFlat()) {
		Query *query = new Query(t);
		querys->push_back(query);
	}

	fclose(fp);
}
void Data::readData()
{
	Transaction *pre;
	in = fopen(datafile,"rt");
	pre = getNext();//读取第一行字符
   qsorta(pre->t,0,pre->length-1);//对读取的一行字符进行排序
	pre->id =transcount++;
	this->trans = pre;
	while (Transaction *t=getNext())//循环读取in中的字符，直到全部读取结束
	{
		if(t->length) {
		qsorta(t->t,0,t->length-1);
		t ->id = transcount++;
		pre->next = t;
		pre = t;
			}
		}

	pre->next = NULL;
	fclose(in);

}

void Data::sortData(int type,set<Element> *relist,int *remap)
{
	//lex order
	if(type==LEX_ORDER)
	{
		Transaction * pre= trans;
		while(pre)
		{
			qsorta(pre->t,0,pre->length-1);
			//heap_sort(pre->t,pre->length);
			pre=pre->next;
		}
	}
	else if(type ==WORDFRE_ORDER)//word frequency order
	{
		Transaction * pre= trans;
		while(pre)
		{
			vector<int> list;
			int i=0;

			for(i =0;i<pre->length;i++)
			{
				set<Element>::iterator it = relist->find(Element(pre->t[i],0));
				if(it != relist->end()) list.push_back(it->id);
			}
			sort(list.begin(),list.end());


			for(i=0;i<pre->length;i++)
			{
				//pre->t[i] = remap[list[i]];

				pre->t[i] = list[i];
				//pre->t[i] = list[i];
			}
			pre=pre->next;
		}
	}


}

Transaction *Data::getNext1() {
	int bytes_read;
	int size;
	int id;
	Transaction *t;

	while ((bytes_read = fread(&id, 1, 4, in)) == 4) {
		bytes_read = fread(&size, 1, 4, in);
		if (bytes_read != 4) {

			return NULL;
		}
		t=new Transaction(size);
		t->id = id;
		bytes_read = fread(t->t, 1, 4 * size, in);
		if (bytes_read != 4 * size) {
			if (ferror(in))
				break;
			return NULL;
		}
		//lines_processed_++;
		return t;
	}
	if (ferror(in)) {
		//error_ = "Dataset read error, ferror code=" + ToString(ferror(in));
		return  NULL;
	}
	if (bytes_read != 0) {
		//error_ = "Dataset format error. Partial vector id encountered.";
		return NULL;
	}
	return NULL;
}
Transaction *Data::getNext()
{
	Transaction *t=0;

	switch(type){
  case 1: t= getNextBin(); break;
  case 2: t= getNextAs(); break;
  case 3: t= getNextAsFlat(); break;
  case 4: t= getNextAsQuest(); break;

	}

	if(t) current++;
	else {
		rewind(in);
		current=0;
	}

	return t;
}

Transaction *Data::getNextAs()
{
	Transaction *t;
	int tid, item, i, dummy;
	vector<int> list;
	static int cur=0,prev=-1;
	static bool begin=true;

	if(feof(in)) {
		begin=true;
		prev=-1;
		return 0;
	}

	if(!begin) list.push_back(cur);
	else begin=false;

	while(true) {
		fscanf(in, "%d %d %d",&dummy, &tid, &item);
		if(feof(in)) {
			int size=list.size();
			t = new Transaction(size);
			for(i=0; i<size; i++) t->t[i] = list[i];
			list.clear();

			return t;
		}
		else if(prev<0)	prev=tid;
		else if(tid != prev){
			prev = tid;
			cur = item;
			int size=list.size();
			t = new Transaction(size);
			for(i=0; i<size; i++) t->t[i] = list[i];
			list.clear();

			return t;
		}

		list.push_back(item);
	}
}
void Data::getDataInfo()
{
	set<int> distinctTerms;
	set<int>::iterator it;
	int count =0,maxlen=0,minlen=1000000;
	int totalcount=0;
	Transaction *pre = trans;
	do
	{
		totalcount += pre->length;
		for(int i=0;i<pre->length;i++)
		{
			it = distinctTerms.find(pre->t[i]);
				if(it==distinctTerms.end())
					distinctTerms.insert(pre->t[i]);
		}
		if(pre->length>maxlen) maxlen =pre->length;
		if(pre->length<minlen) minlen=pre->length;
		pre= pre->next;
		count++;
	}
	while(pre !=NULL);
	info = new DataInfo(count,totalcount,maxlen,minlen,distinctTerms);
}
void Data::getMswebData(FILE *rawin,FILE *out)
{
	vector<int> list;
	//set<int> distinctTerms;
	//set<int>::iterator it;
	//char result[5];
	int result;
	int flag =-1;
	int count =0;
	//int count =0,maxlen,minlen=100000;

	Transaction *pre;
	do
	{
		result = readMswebSingleLine(rawin,&flag);
		if((flag==0 && list.size()>0) || (flag==-1 && list.size()>0))
		{
			Transaction *t = new Transaction(list.size());
			for(int i=0;i<list.size();i++)
			{
				t->t[i]=list[i];
				if(out!=NULL)
					fprintf(out, "%d ", list[i]);

				//it = distinctTerms.find(list[i]);
				//if(it==distinctTerms.end())
				//	distinctTerms.insert(list[i]);
			}
		/*	if(list.size()>maxlen) maxlen =list.size();
			if(list.size()<minlen) minlen=list.size();*/

			if(out!=NULL)
				fprintf(out,"%c",'\n');
			t->id = count;
			if(count==0)
			{
				pre = trans = t;
			}
			else
			{
				if(flag==-1) t->next =NULL;
				pre->next =t;
				pre=t;
			}

			count ++;
			list.clear();
		}
		else if(flag==1)
		{
			list.push_back(result);
		}

	}
	while(flag!=-1);

	//info = new DataInfo(count,maxlen,minlen,distinctTerms);

}

int Data::readMswebSingleLine(FILE *rawin,int *flag)
{
	vector<int> list;
	char c;
	bool startnew = false;
	char result[5];

	int res=-1;
	c = getc(rawin); //读一行的第一个字符
	if(feof(rawin))
	{
		*flag = -1;
		return res;
	}
	if(c!='C'&& c!='V') //读取非case行和非vroot行
	{
		*flag=2;
		while(c!='\n' && !feof(rawin))
		{
			c=getc(rawin);
		}
	}
	else if(c=='C') //读取case行
	{
		*flag = 0;
		while(c!='\n' && !feof(rawin))
		{
			c=getc(rawin);
		}
	}
	else if(c=='V') //读取vroot行
	{
		int pos = 0;

		*flag = 1;
		c=getc(rawin);//读逗号
		c=getc(rawin);
		//读逗号后第一个字符
		while(c != ',')
		{
			result[pos]=c;
			c=getc(rawin);
			pos++;
		}
		result[pos] ='\0';
		res = atoi(result);
		while(c!='\n' && !feof(rawin))
		{
			c=getc(rawin);
		}
	}

	return res;

}
Transaction *Data::getNextAsFlat()
{
	vector<int> list;
	char c;

	// read list of items
	do {
		int item=0, pos=0;
		c = getc(in);
		while((c >= '0') && (c <= '9')) {
			item *=10;
			item += int(c)-int('0');
			c = getc(in);
			pos++;
		}
		if(pos) list.push_back(item);
	}while(c != '\n' && !feof(in));

	// if end of file is reached, rewind to beginning for next pass
	if(feof(in)){
		rewind(in);
		return 0;
	}

	Transaction *t = new Transaction(list.size());
	for(int i=0; i<int(list.size()); i++)
		t->t[i] = list[i];

	return t;
}

Transaction *Data::getNextAsFlat(FILE *fp)
{
	vector<int> list;
	char c;

	// read list of items
	do {
		int item=0, pos=0;
		c = getc(fp);
		while((c >= '0') && (c <= '9')) {
			item *=10;
			item += int(c)-int('0');
			c = getc(in);
			pos++;
		}
		if(pos) list.push_back(item);
	}while(c != '\n' && !feof(fp));

	// if end of file is reached, rewind to beginning for next pass
	if(feof(fp)){
		rewind(fp);
		return 0;
	}

	// put items in Transaction structure
	Transaction *t = new Transaction(list.size());
	for(int i=0; i<int(list.size()); i++)
		t->t[i] = list[i];

	return t;
}


Transaction *Data::getNextAsFlatWithID(FILE *fp)
{
	vector<int> list;
	char c;

	// read list of items
	do {
		int item=0, pos=0;
		c = getc(fp);
		while((c >= '0') && (c <= '9')) {
			item *=10;
			item += int(c)-int('0');
			c = getc(fp);
			pos++;
		}
		if(pos) list.push_back(item);
	}while(c != '\n' && !feof(fp));

	// if end of file is reached, rewind to beginning for next pass
	if(feof(fp)){
		rewind(fp);
		return 0;
	}

	//第一个数为id
	Transaction *t = new Transaction(list.size()-1);
	t->id = list[0];
	for(int i=1; i<int(list.size()); i++)
		t->t[i-1] = list[i];

	return t;
}

Transaction *Data::getNextAsQuest()
{
	int tmptid, tid,l,i;
	Transaction *t;

	fscanf(in,"%d %d %d",&tmptid,&tid,&l);
	if(feof(in)) return 0;

	t = new Transaction(l);
	for(i=0; i<l; i++) fscanf(in,"%d",&t->t[i]);
	return t;
}

Transaction *Data::getNextBin()
{
	int tmptid, tid,l,i;
	Transaction *t;

	fread(&tmptid,4, 1,in);
	if(feof(in)) return 0;

	fread(&tid,4, 1,in);
	fread(&l,4, 1,in);
	t = new Transaction(l);
	for(i=0; i<l; i++) fread(&t->t[i],4, 1,in);

	return t;
}
//从数据库中随机抽取querycount条记录，并写入queryfile文件。
vector<Transaction *>  *Data::generatQuery(int querycount,char *queryfile)
{
	vector<Transaction *> querys;
	Transaction *t = this->trans;
	int transcount =0;
	while(t!=NULL)
	{
		transcount++;
		t = t->next;
	}

	int *tranids = getNonRepeat(transcount,querycount);
	qsorta(tranids,0,querycount-1);
	for(int i=0;i<querycount;i++)
	{
		printf("%d ",tranids[i]);
		if(i%10==0)
			printf("\n");
	}
	int pos = 0;
	t = this->trans;
	while(t!=NULL)
	{
		if(t->id < tranids[pos])
			t = t->next;
		else if(t->id > tranids[pos])
		{
			pos++;
		}
		else
		{
			querys.push_back(t);
			t=t->next;
			pos++;
		}
	}
	FILE *fp=fopen(queryfile,"w");
	for(int i=0;i<querycount;i++)
	{
		for(int j =0;j<querys[i]->length;j++)
		{
			if(j!= querys[i]->length -1)
				fprintf(fp,"%d ",querys[i]->t[j]);
			else
				fprintf(fp,"%d",querys[i]->t[j]);

		}
		fprintf(fp,"\n");
	}

	fclose(fp);

	return &querys;
}
//从数据集中为每一个阈值分别随机抽取长度大于等于阈值的查询querycount条，并将查询写入queryfile
//如果满足特定阈值的记录不足querycount条，我们简单地重复加入已有的查询记录。
void Data::generatQueryThreshold(vector<Transaction *> *querys,int querycount,char *queryfile,int *threshold,int count)
{
	Transaction *t = this->trans;
	int transcount =0;
	int multiple ;
	int transg[5]; //记录大于等于各阈值的记录数
	for(int i =0;i<count;i++)
	{
		transg[i] = 0;
	}
	while(t!=NULL)
	{
		transcount++;
	    for(int i=0;i<count;i++)
		{
			if (t->length >=threshold[i])
			{
				transg[i]++;
			}
			else
			{
				break;
			}
		}

		t = t->next;
	}

	for(int k=0;k<count;k++)
	{
		//如果满足特定阈值的记录不足querycount条，我们简单地重复加入已有的查询记录。
		if(transg[k] < querycount)
		{
			t = this->trans;
			while(t!=NULL)
			{
				if(t->length >= threshold[k])
				{
					querys->push_back(t);
				}
				t=t->next;
			}
			if(transg[k]!=0)
			  multiple = querycount  / transg[k];

			for(int m=0;m<multiple;m++)
			{
				for(int i = k * querycount;i<k * querycount + transg[k];i++)
				{
					querys->push_back(querys->at(i));
					if(querys->size()>=(k+1)*querycount)
						break;
				}
			}
		}
		else
		{
			int *tranids = getNonRepeat(transg[k],querycount);
			qsorta(tranids,0,querycount-1);
			for(int i=0;i<querycount;i++)
			{
				printf("%d ",tranids[i]);
				if((i+1)%10==0)
					printf("\n");
			}
			int qualifiedid = 0;
			int pos = 0;
			t = this->trans;
			while(t!=NULL)
			{
				if (t->length < threshold[k])
				{
					t = t->next;
					continue;
				}

				if(qualifiedid < tranids[pos])
					t = t->next;
				else if(qualifiedid > tranids[pos])
				{
					pos++;
				}
				else if(qualifiedid == tranids[pos])
				{
					querys->push_back(t);
					t=t->next;
					pos++;
				}
				qualifiedid++;
			}
		}

	}

	FILE *fp=fopen(queryfile,"w");
	for(int i=0;i<querycount*count;i++)
	{
		fprintf(fp,"%d ",querys->at(i)->id);
		for(int j =0;j<querys->at(i)->length;j++)
		{
			if(j!= querys->at(i)->length -1)
				fprintf(fp,"%d ",querys->at(i)->t[j]);
			else
				fprintf(fp,"%d",querys->at(i)->t[j]);

		}
		fprintf(fp,"\n");
	}

	fclose(fp);
}
void Data::mulitpleTrans(vector<Transaction *> *querys,int countnow,int countfinal)
{
   	int multiple = countfinal  / countnow;

	for(int m=0;m<multiple;m++)
	{
		for(int i = 0;i< countnow;i++)
		{
			querys->push_back(querys->at(i));
			if(querys->size()>=countfinal)
				break;
		}
	}
}
//选择合适的transaction来构造查询
//query：存放适合的transaction指针
//count：要生成的查询数量
//minlen：满足条件的transaction的长度下限
//rules：选择规则，0，前count个满足条件的transaction；1：随机count个满足条件的transaction；
//qualifiednum:符合长度大于minlen的transaction的数量
//currentK：当前threshold或cardinality序号
void Data::selectTrans(vector<Transaction *> *querys,int count,int minlen,int qualifiednum,int rules)
{
	Transaction *t = this->trans;
	int num = 0;
	if(qualifiednum <count)
	{
		while(t!=NULL)
		{
			if(t->length >= minlen)
			{
				querys->push_back(t);
			}
			t= t->next;
		}
		mulitpleTrans(querys,qualifiednum,count);
	}
	else if(rules==0) //选择前count个满足minlen的transaction
	{
	 	while(num <count)
		{
			if(t->length >= minlen)
			{
				querys->push_back(t);
				num++;
			}
			t= t->next;
		}
	}
	else if(rules ==1) //随机选择count个满足minlen的transaction
	{
		int *tranids = getNonRepeat(qualifiednum,count);
		qsorta(tranids,0,count-1);
		/*for(int i=0;i<querycount;i++)
		{
			printf("%d ",tranids[i]);
			if((i+1)%10==0)
				printf("\n");
		}*/
		int qualifiedid = 0;
		int pos = 0;
		t = this->trans;
		while(t!=NULL)
		{
			if (t->length < minlen)
			{
				t = t->next;
				continue;
			}

			if(qualifiedid < tranids[pos])
				t = t->next;
			else if(qualifiedid > tranids[pos])
			{
				pos++;
			}
			else if(qualifiedid == tranids[pos])
			{
				querys->push_back(t);
				t=t->next;
				pos++;
			}
			qualifiedid++;
		}
	}
}
//根据特定集合构造查询
//rules:
void Data::addTranToQuery(Transaction * t,vector<Transaction *> *querys,int len,int rules)
{
	if(rules == 0) //t的整体作为查询
	{
		querys->push_back(t);
	}
	else if(rules ==1)//t的前len个元素构成一个查询
	{
		Transaction  *newt = new Transaction(len);
		newt->id = t->id;
		for(int i =0;i<len;i++)
		{
			newt->t[i] = t->t[i];
		}
		querys->push_back(newt);
	}
	else if(rules ==2)//t的随机len个元素构成一个查询
	{
		int *elempos = getNonRepeat(t->length,len);
		qsorta(elempos,0,len-1);
		int pos =0;
		Transaction  *newt = new Transaction(len);
		newt->id = t->id;
		for(int i =0;i<t->length;i++)
		{
			if(i == elempos[pos])
			{
				newt->t[pos] = t->t[i];
				pos++;
			}

		}
		querys->push_back(newt);
	}

}

//从数据集中为每一个阈值分别随机抽取长度大于等于阈值的查询querycount条，并将查询写入queryfile
//如果满足特定阈值的记录不足querycount条，我们简单地重复加入已有的查询记录。
void Data::generatQueryOK(vector<Transaction *> *querys,int querycount,char *queryfile,int *cardinality,int count,int srules,int trules,int includeID)
{
	vector<Transaction *> *tempquerys = new vector<Transaction *>;
	Transaction *t = this->trans;
	int transcount =0;
	int transg[6]; //记录大于等于各势的记录数
	for(int i =0;i<count;i++)
	{
		transg[i] = 0;
	}
	while(t!=NULL)
	{
		transcount++;
	    for(int i=0;i<count;i++)
		{
			if (t->length >=cardinality[i])
			{
				transg[i]++;
			}
			else
			{
				break;
			}
		}

		t = t->next;
	}

	for(int k=0;k<count;k++)
	{
		tempquerys->clear();
		selectTrans(tempquerys,querycount, cardinality[k],transg[k],srules);
		for(int j=0;j<tempquerys->size();j++)
			addTranToQuery(tempquerys->at(j),querys,cardinality[k],trules);
	}
	delete tempquerys;

	FILE *fp=fopen(queryfile,"w");
	for(int i=0;i<querycount*count;i++)
	{
		if(includeID ==1)
			fprintf(fp,"%d ",querys->at(i)->id);
		for(int j =0;j<querys->at(i)->length;j++)
		{
			if(j!= querys->at(i)->length -1)
				fprintf(fp,"%d ",querys->at(i)->t[j]);
			else
				fprintf(fp,"%d",querys->at(i)->t[j]);

		}
		fprintf(fp,"\n");
	}

	fclose(fp);
}

void Data::generateDuplicate(double ratio,char *outfile)
{
	FILE *fp = fopen(outfile,"w");

	vector<Transaction *> vec;

	Transaction *t = this->trans;
	int transcount = 0;
	while(t!=NULL)
	{
		transcount++;
		t = t->next;
	}
	int remaindcount = transcount *(1-ratio);
	int repeatcount = transcount - remaindcount;
	int *tranidstoremove = getNonRepeat(transcount,repeatcount);
	qsorta(tranidstoremove,0,repeatcount-1);

	int pos =0;
	t = this->trans;
	while(t!=NULL)
	{
		if(t->id != tranidstoremove[pos])
		{
			vec.push_back(t);
		}
		else
		{
			pos++;
		}
		t = t->next;
	}

	int *trannostoadd = getNonRepeat(remaindcount,repeatcount);
	qsorta(trannostoadd,0,repeatcount -1);

	for(int i = 0;i <repeatcount;i++)
	{
		Transaction * newt  = new Transaction(* vec.at(trannostoadd[i]));
		vec.push_back(newt);
	}


	for(int i = 0;i<vec.size();i++)
	{
		for(int j =0;j<vec[i]->length;j++)
		{
			if(j!= vec[i]->length -1)
				fprintf(fp,"%d ",vec[i]->t[j]);
			else
				fprintf(fp,"%d\n",vec[i]->t[j]);

		}
	}

	fclose(fp);

}

void Data::reverseQuery(vector<Transaction *> *querys,char *remapfile,char *newqueryfile,int includeID)
{

	FILE * fp = fopen(remapfile,"r");
	vector<int> list;
	char c;

	// read list of items
	do {
		int item=0, pos=0;
		c = getc(fp);
		while((c >= '0') && (c <= '9')) {
			item *=10;
			item += int(c)-int('0');
			c = getc(fp);
			pos++;
		}
		if(pos) list.push_back(item);
	}while(c != '\n' && !feof(fp));

	fclose(fp);

	FILE *newfp = fopen(newqueryfile,"w");
	for(int i =0;i<querys->size();i++)
	{
		for(int j=0;j<querys->at(i)->length;j++)
		{
			querys->at(i)->t[j] = list[querys->at(i)->t[j] -1];
		}
		qsorta(querys->at(i)->t,0,querys->at(i)->length -1);
	}

	for(int i =0;i<querys->size();i++)
	{
		if(includeID ==1)
			fprintf(newfp,"%d ",querys->at(i)->id);
		for(int j=0;j<querys->at(i)->length;j++)
		{
			if(j==querys->at(i)->length -1)
			{
				fprintf(newfp,"%d\n",querys->at(i)->t[j]);
			}
			else
			{
				fprintf(newfp,"%d ",querys->at(i)->t[j]);
			}
		}
	}
	fclose(newfp);

}

