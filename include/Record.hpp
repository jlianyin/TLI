#ifndef RECORD_HPP_INCLUDED
#define RECORD_HPP_INCLUDED
#include <set>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
namespace TLI{
struct Record
{

	Record() {}
	Record(int id) : id_(id) {}

	int& operator[](int i)
	{
		return list_[i];
	}

    int operator[](int i)const
	{
		return list_[i];
	}

	void AddNum(int num)
	{
		list_.push_back(num);
	}

	void SortList(map<int, int>& freqMap)
	{
		sort(list_.begin(), list_.end(), [&freqMap](int a, int b)->bool{
			if (freqMap[a] == freqMap[b])
				return a < b;
			else
				return freqMap[a] > freqMap[b];
		});
	}

//	void Print()
//	{
//		cout << "Id: " << id_ << endl;
//		cout << "List: ";
//		for (auto it = list_.begin(); it != list_.end(); ++it)
//			cout << *it << " ";
//		cout << endl << endl;
//	}

//	friend bool IsEqualRecord(Record& lhs, Record& rhs);

	int compare(const Record& keys)const
	{
        if(list_.size() != keys.list_.size())
            return 1;

        auto it1 = list_.begin();
        auto it2 = keys.list_.begin();
        for(; it1 != list_.end() && it2 != keys.list_.end(); ++it1, ++it2){
            if(*it1 != *it2)
                return 1;
        }
        return 0;
	}

	int length() const
	{
        return list_.size();
	}

	void ReplaceList(map<int, int>& freqIdx)
	{
       for(size_t i = 0; i < list_.size(); ++i)
       {
        list_[i] = freqIdx[list_[i]];
       }
	}

	int id_;
	vector<int> list_;

};
//inline bool IsEqualRecord(Record& lhs, Record& rhs)
//{
//	auto it1 = lhs.list_.begin();
//	auto it2 = rhs.list_.begin();
//	while (it1 != lhs.list_.end() && it2 != rhs.list_.end()) {
//		if (*it1 != *it2)
//			return false;
//		++it1;
//		++it2;
//	}
//
//	if (it1 == lhs.list_.end() && it2 == rhs.list_.end())
//		return true;
//	return false;
//}

}

#endif // RECORD_HPP_INCLUDED
