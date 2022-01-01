#ifndef TRIE_H
#define TRIE_H

#include <set>
#include <queue>
#include <string>
#include "elem.h"
#include "query.h"
#include "trienode.hpp"
#include "item.h"
#include "util.h"
#include "Record.hpp"
#include "LOUDS_Flat.hpp"

using namespace std;
namespace TLI
{

#define MAX_ELEM_SIZE 60000000
class Trie
{
public:

    Trie(int c) ;

    ~Trie() {
    delete louds;

	for(vector<TrieNode *>::iterator it = root->begin(); it != root->end(); it++)
       (*it)->removeChildren();

	delete root;



    };

    void insertKeyIntoTrie(Record &key, int dupCount, bool peFlag);
    void SSQInTrie(Record &query);
    void upperSearch(vector<TrieNode *> *current,Record &query,int level,vector<ENIL> &res);
    void superSetQuery(Record &query,vector<ENIL> &res);
    void setLOUDS(LOUDS_Flat *l)
    {
        louds = l;
    }

public:
    vector<TrieNode *> *root;
    TrieNode *queryNode;
    uint32_t pNodeCount;
    int cutOffLevel;

    uint32_t nodeCount;
    uint32_t eNodeCount;
    uint32_t peNodeCount;
    LOUDS_Flat *louds;
    FILE *resFp;

	uint32_t totalFound;
	uint32_t totalAccessed;

};

Trie::Trie(int c)
{
    root = new vector<TrieNode *>;
    cutOffLevel = c;
    nodeCount = 0;
    pNodeCount = 0;
    eNodeCount = 0;
    peNodeCount =0;

    totalFound = 0;
    totalAccessed=0;
}

void Trie::superSetQuery(Record &query,vector<ENIL> &res)
{
    if(cutOffLevel ==0)
        louds->lowerSearch(query,0,0,res);

    else
        upperSearch(root,query,0,res);
}

void Trie::upperSearch(vector<TrieNode *> *current,Record &query,int level,vector<ENIL> &res)
{
    while(level< query.length() )
    {
        TrieNode *queryNode= new TrieNode(query[level]);
        int mid = binaryPointerWithCompareNumber<TrieNode *>(queryNode,&((*current)[0]),current->size());

        totalAccessed += 1;
        if(mid!=-1)
        {
            int pNodeNum;
            TrieNode *tn = (*current)[mid];
            //totalfound += tn->getENIL()->getArray()->size();
            if(tn->type == 1)
            {
                res.push_back(((ENode *)tn)->getENIL());
                totalFound += ((ENode *)tn)->getENIL().dupCount;
                //fprintf(resFp,"%d, %d\n",((ENode *)tn)->getENIL().startID,((ENode *)tn)->getENIL().dupCount);
            }

            else if(tn->type == 2)
                pNodeNum = ((PNode *)tn)->getPNum();
            else if (tn->type == 3)
            {
                res.push_back(((PENode *)tn)->getENIL());
                pNodeNum = ((PENode *)tn)->getPNum();
                totalFound += ((PENode *)tn)->getENIL().dupCount;
                //fprintf(resFp,"%d, %d\n",((ENode *)tn)->getENIL().startID,((ENode *)tn)->getENIL().dupCount);
            }
            if(tn->getChildren() != NULL)  // not a leaf node
            {

                  upperSearch(tn->getChildren(),query,level+1,res);
            }
            else if(tn->type>=2) //Pnode or PENode
            {
                 louds->lowerSearch(query,level+1,pNodeNum,res);
            }


        }
        level++;
    }
    // return true;
}

void Trie::SSQInTrie(Record &query)
{
    vector<ENIL> res;
    upperSearch(root,query, 0, res);
}

void Trie::insertKeyIntoTrie(Record &key, int dupCount, bool peFlag)
{
    //vector<TrieNode>::iterator it;
    vector<TrieNode *>* trienodes = root;

    TrieNode *current = NULL;

    for(int depth=0; depth < cutOffLevel; depth++)
    {
        if(depth == cutOffLevel-1 && peFlag==true)
        {
            //if there are some duplicates records existing, the key.id_ is last id of these record,
            // so we can restore startID by key.id_- dupCount + 1
            PENode *tn = new PENode(key[depth], pNodeCount++, ENIL(key.id_- dupCount + 1, dupCount));
            //ordered_insert(*trienodes,tn,NodePointerOrder());
            trienodes->push_back(tn);
            peNodeCount++;
            nodeCount++;
            //fprintf(resFp,"%d, %d\n",((ENode *)tn)->getENIL().startID,((ENode *)tn)->getENIL().dupCount);
            //return pNodeCount; // this key ends in upper.
            return;

        }
        //As we insert keys in dictionary order,so if depth == key.size()-1, this node must have not in trie.
        else if(depth == key.length()-1) //leaf node, also ENode
        {
            ENode* tn = new ENode(key[depth], ENIL(key.id_- dupCount + 1, dupCount));
            eNodeCount++;
            nodeCount++;
            trienodes->push_back(tn);
            return;

            //return -1; // this key ends in upper.
        }
        else if(depth== cutOffLevel-1) //PNode
        {
            if(trienodes->size()==0 || (*trienodes)[trienodes->size()-1]->elem != key[depth])
            {
                PNode *tn = new PNode(key[depth],pNodeCount++);
                //pNodeCount++;
                nodeCount++;
                trienodes->push_back(tn);
            }
            //return pNodeCount;
            return;
        }
        else //inner node in upper
        {

            TrieNode *tn = NULL;

            if(trienodes->size()==0 || (*trienodes)[trienodes->size()-1]->elem != key[depth])
            {
                tn = new TrieNode(key[depth]);
                trienodes->push_back(tn);
                nodeCount ++;
            }
            else
            {
                tn = (*trienodes)[trienodes->size()-1];
            }
            trienodes = tn->makeChildren();
        }
    }
    //return -2; // only when cutofflevel is 0, it will return this value
}


}
#endif TRIE_H
