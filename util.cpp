#include "include/util.h"
#include <stdio.h>

void listMerge(vector<int> *v1,vector<int> *v2,vector<int> *v)
{
	vector<int>::iterator it1,it2;
	it1 = v1->begin();
	it2=v2->begin();
	while(it1!=v1->end() && it2!= v2->end())
	{
		if(*it1 < *it2)
		{
			v->push_back(*it1);
			it1++;
		}
		else
		{
			v->push_back(*it2);
			it2++;
		}
	}
	while(it1!=v1->end())
	{
		v->push_back(*it1);
			it1++;
	}
	while(it2!=v2->end())
	{
		v->push_back(*it2);
			it2++;
	}
}
/*
利用binary算法在有序数据序列data中查找第一个大于key的元素的位置，返回位置

*/
int binarygreaterpos(int key,int *data,int length,bool canequlity)
{
	int low=0, high = length -1;
	int mid =-1;
	if(key>data[high]) return -1;
	if(key<data[low]) return -1;
	while(low<=high)
	{
		mid = (low + high)/2;
		if(data[mid] == key)
		{
			if(canequlity)
				return mid;
			else
				return mid +1;
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
	return low;
}

/*
利用binary算法在有序数据序列data中查找第一个大于key的元素的位置，返回位置

*/
int binarygreaterposWithComparedTimes(int key,int *data,int length,int *comparetimes,bool canequlity)
{
	int low=0, high = length -1;
	int mid =-1;
	if(key>data[high]) return -1;
	if(key<data[low]) return -1;
	while(low<=high)
	{
		mid = (low + high)/2;

		*comparetimes +=1;
		if(data[mid] == key)
		{
			if(canequlity)
				return mid;
			else
				return mid +1;
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
	return low;
}

bool isFileExist(char *fn)
{
	FILE *fp;
	fp = fopen(fn,"r");
	if(fp==NULL)
		return false;
	else
	{
		fclose(fp);
		return true;
	}
}
int *getNonRepeat(int maxval,int count)
{
	srand((unsigned)time(NULL));

	int *a = new int[maxval];
	int *b = new int[count];
	uint32_t i, m;
	for(i=0;i<maxval;i++)
		a[i] = 0;
	for(i=1; i<maxval; ++i)
	{
		do
		{
			if(maxval<32768)
				m = random(maxval);
			else
			{
			m = (rand()<<5 ^ rand()) ;
			m %=  maxval;
			}

		}
		while(a[m]);
		a[m] = i;
	}
	for(i=0;i<count;i++)
		b[i] = a[i];

	delete []a;

	return b;
}

template <class T>
int binarySearch(T* data, int pos1, int pos2, T key)
{
    int low=pos1, high =pos2,mid;
    while (low <=high)
    {
        mid = (low + high)/2;
        if(data[mid] == key)
            return mid;
        else if(data[mid]> key)
            high = mid -1;
        else
            low = mid +1;
    }
    return -1;
}

bool search(int *data,int key,int size)
{
	for(int i=0;i<size;i++)
	{
		if(data[i] == key) return true;
	}
	return false;
}

int *getRandom(int maxval,int count)
{
	srand((unsigned)time(NULL));
	int *a = new int[count];
	int i=0,r=0,len=0;

	while(true)
	{
		r = random(maxval);
		if(r<0 || r>=maxval) continue;
		if(!search(a,r,len))
		{
			a[len++] =r;

			if(len>=count) return a;
		}
	}

	return a;
}


/* 实现有序化vector*/
//template <typename Container>
//inline void ordered_insert(Container& c,typename Container::value_type const& t)
//{
//    c.insert(std::upper_bound(c.begin(),c.end(),t),t);
//}
//
//template <typename Container, typename Cmp>
//inline void ordered_insert(Container& c,typename Container::value_type const& t,Cmp cmp)
//{
//	c.insert(std::upper_bound(c.begin(),c.end(),t,cmp), t);
//}
//
//template <typename Container,typename It>
//inline void erase_range(Container& c, std::pair<It, It> const& r)
//{
//	c.erase(r.first, r.second);
//}
//
//template <typename Container>
//inline void ordered_erase(Container& c,typename Container::value_type const& t)
//{
//	erase_range(c, std::equal_range(c.begin(),c.end(), t));
//}
//
//template <typename Container,typename T, typename Cmp>
//inline void ordered_erase(Container& c, T const& t, Cmp cmp)
//{
//	erase_range(c, std::equal_range(c.begin(), c.end(), t, cmp));
//}

int partition(int *a,int l,int r)
{
    int i,j,temp;

    i=l;j=r;
    temp=a[i];
    do{
       while((a[j]>temp) && (i<j))
           j--;
       if(i<j)
           swap(a[i++],a[j]);
       while((a[i]<=temp) && (i<j))
           i++;
       if(i<j)
           swap(a[j--],a[i]);
    }while(i!=j);
    a[i]=temp;
 return i;
}


void qsorta(int* a,int l,int r)
{
    if(l<r){
            int i = partition(a,l,r);
            qsorta(a,l,i-1); //对左半段排序
            qsorta(a,i+1,r); //对右半段排序
    }
}
