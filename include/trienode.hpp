#ifndef TRIENODE_H
#define TRIENODE_H
#include <vector>
#include "config.hpp"
using namespace std;
namespace TLI{

class TrieNode
{
public:
	TrieNode() {type = 0; children = NULL;}
	TrieNode(uint32_t e) {elem = e; type = 0; children = NULL;}

	//TrieNode(int e,TrieNode *p) {elem = e; this->parent = p; children = NULL;}
	uint32_t getElem() const {return elem;}
	void setElem(uint32_t e){elem=e;}

	vector<TrieNode *> *getChildren() const {return children;}


	vector<TrieNode *> *makeChildren();
	~TrieNode();

	void removeChildren();
	bool operator < (const TrieNode &i) const {return getElem() < i.getElem();}
	bool operator > (const TrieNode &i) const {return getElem() > i.getElem();}
	bool operator == (const TrieNode &i) const {return getElem() == i.getElem();}


public:
	uint32_t elem;
	uint8_t type;  //0： upper inner node; 1: enode; 2: pnode;3 EPNode


	vector<TrieNode *> *children;

};

class ENode : public TrieNode
{
public:
	ENode();
	ENode(uint32_t el):TrieNode(el){type=1;};
	ENode(uint32_t el, ENIL en) : TrieNode(el) { enil = en; type = 1;}

	ENIL getENIL() {return enil;}

	void setENIL(ENIL e){enil=e;}


private:
	ENIL enil;
};

class PNode : public TrieNode
{
public:
	PNode();
	PNode(uint32_t el):TrieNode(el){type = 2;};
	PNode(uint32_t el, uint32_t pn) : TrieNode(el) {pnum = pn;type = 2;}

	uint32_t getPNum() {return pnum;}

	void setPNum(uint32_t e){pnum=e;}


private:
	uint32_t pnum;
};

class PENode : public TrieNode
{
public:
	PENode();
	PENode(uint32_t el):TrieNode(el){type = 3;};
	PENode(uint32_t el,uint32_t pn, ENIL en) : TrieNode(el) { pnum = pn; enil = en; type = 3; }

	ENIL getENIL() {return enil;}
	void setENIL(ENIL e){enil=e;}

	uint32_t getPNum() {return pnum;}
	void setPNum(uint32_t e){pnum=e;}


private:
	ENIL enil;
	uint32_t pnum;
};
void TrieNode::removeChildren()
{
  vector<TrieNode *> *nodes = children;

  if(nodes != NULL)
  {
	  for(vector<TrieNode *>::iterator it = nodes->begin();it != nodes->end(); it++)
		  (*it)->removeChildren();
	  delete children;
	  children = NULL;
  }

}
vector<TrieNode *> *TrieNode::makeChildren()
{
	if(children==NULL)
		children = new vector<TrieNode *>;
	return children;
}


TrieNode::~TrieNode()
{
	if(children)
		delete children;
}

}
#endif TRIENODE_H
