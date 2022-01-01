#ifndef ITEM_H
#define ITEM_H
#include <set>

using namespace std;
namespace TLI{
class Item;

class Item_
{
 public:

  Item_();
  ~Item_();


  int id;
  int supp;

  set<Item> *children;

};

class Item
{
 public:

  Item(int s, Item_ *p);
  Item(const Item& i);
  ~Item();

  int getId() const {return item->id;}
  int getSupport() const {return item->supp;}

  set<Item> *getChildren() const {return item->children;}
  set<Item> *makeChildren() const;

  Item_ *getItem() const {return item;}
  bool isFrequent(int ms) const {return item->supp >= ms;}
  void Increment(int i=1) const {item->supp += i;}

  void removeChildren() const;

  bool operator< (const Item &i) const {return getId() < i.getId();}

 private:

  Item_ *item;
};
}

#endif ITEM_H
