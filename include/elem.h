#ifndef ELEM_H
#define ELEM_H
 namespace TLI{
class Element
{
 public:
  Element(int s, int i) : support(s), id(i){}

  int support;
  int id;
  bool operator< (const Element  &e) const {return support > e.support;}
};
}
#endif ELEM_H
