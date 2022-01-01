#ifndef UTIL_H
#define UTIL_H
#include   <stdlib.h>
#include   <stdio.h>
#include   <time.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <stdint.h>

#define random(x) (rand()%x)

using namespace std;


template <typename T>


int binary(T key,T *data,int length)
{
	int low=0, high = length -1;
	int mid;
	if(key>data[high] || key<data[low]) return -1;
	while(low<=high)
	{
		mid = (low + high)/2;
		if(data[mid] == key)
			return mid;
		else if(data[mid]<key)
		{
			low = mid+1;
		}
		else
		{
			high =mid-1;
		}

	}
	return -1;
}

template <typename T>
int binaryWithComparedTimes(T key,T *data,int length,int *comparetimes)
{
	int low=0, high = length -1;
	int mid;
	if(key>data[high] || key<data[low]) return -1;
	while(low<=high)
	{
		mid = (low + high)/2;
		*comparetimes +=1;
		if(data[mid] == key)
		{
			return mid;
		}
		else if(data[mid]<key)
		{
			low = mid+1;
		}
		else
		{
			high =mid-1;
		}

	}
	return -1;
}

template <typename T>
int binaryPointer(T key,T *data,int length)
{
	int low=0, high = length -1;
	int mid;
	if(*key>*data[high] || *key<*data[low]) return -1;
	while(low<=high)
	{
		mid = (low + high)/2;
		if(*data[mid] == *key)
			return mid;
		else if(*data[mid]<*key)
		{
			low = mid+1;
		}
		else
		{
			high =mid-1;
		}

	}
	return -1;
}

template <typename T>
//int binaryPointerWithCompareNumber(T key,T *data,int length, int *count)
int binaryPointerWithCompareNumber(T key,T *data,int length)
{
	int low=0, high = length -1;
	int mid;
	if(*key>*data[high] || *key<*data[low]) return -1;
	while(low<=high)
	{
		mid = (low + high)/2;
		if(*data[mid] == *key)
			return mid;
		else if(*data[mid]<*key)
		{
			low = mid+1;
		}
		else
		{
			high =mid-1;
		}
		//*count += 1;

	}
	return -1;
}


template <typename T>
int scanPointer(T key,T *data,int length)
{
	for(int i=0;i<length;i++)
	{
		if(*data[i] == *key)
		{
			return i;
		}
	}
	return -1;

}
int binarygreaterpos(int key,int *data,int length,bool canequlity=false);

int binarygreaterposWithComparedTimes(int key,int *data,int length,int *comparetimes, bool canequlity=false);

//int doublebinary(int key,InvertList *data);

void qsorta(int* a,int l,int r);

bool isFileExist(char *fn);

int *getNonRepeat(int setsize,int count);
int *getRandom(int maxval,int count);

void listMerge(vector<int> *v1,vector<int> *v2,vector<int> *v);

typedef enum ElementOrder
{
	LEX_ORDER = 0,
	WORDFRE_ORDER
}ElementOrder;


typedef enum QueryType
{
	SUBSET = 0,
	EQULITY,
	SUPERSET,
	SIMILARITY
}QueryType;

typedef enum IndexType
{
	PROBECOUNT = 0,
	PROBECOUNTOPTMERGE,
	PROBECOUNTSORT,
	TRIEINDEX,

}IndexType;

typedef enum DataType
{
	BINARY = 1,
	ASCII,
	FLAT,
	ASCIIBINARY
}DataType;

typedef enum CommonThreshold
{
	TEN =10,
	THIRTY = 30,
	FIFTY=50,
	SEVENTY
}CommonThreshold;

typedef enum ListWhere
{
	LISTINMEMORY,
	LISTNOTINMEMORY
}ListWhere;

typedef enum SimilarityType
{
	JACCARD,
	COSINE,
	DICE,
	NEWOVERLAP,
	HAMMING
}SimilarityType;

#endif UTIL_H
