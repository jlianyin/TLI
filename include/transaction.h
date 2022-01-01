#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <stdio.h>
#include <set>
#include <vector>
using namespace std;
namespace TLI{
class Transaction
{
 public:
  //Transaction();
  Transaction(int l) : length(l) {t = new int[l];}
  Transaction(const Transaction &tr);
  ~Transaction(){delete t;}


  int id;

  int length;
  int *t;
  Transaction *next;
};
}
#endif TRANSACTION_H
