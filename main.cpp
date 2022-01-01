#include <iostream>
#include <vector>
#include <time.h>
#include<sys/time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include "include/data.h"
#include "include/item.h"
#include "include/trie.hpp"
#include "include/LOUDS_Level.hpp"
#include "include/LOUDS_Flat.hpp"
#include "include/query.h"
#include "include/util.h"
using namespace std;
using namespace TLI;

FILE* resFp;
const int cutOffLevel[8] = {0,2,4,6,8,10,15,1000};
bool isSameKey(const Record& a, const Record& b)
{
    return a.compare(b) == 0;
}

level_t computeCommonPrefix(const Record& key1,const Record& key2)
{
    level_t level = 0;

    while (level < key1.length() &&level < key2.length()&& key1[level]==key2[level] )
    {
        level++;
    }
    return level;
}


void createIndex(Trie *trie, LOUDS_Level* louds, vector<Record> keys)
{
    int dupCount = 1;
    bool peFlag = false;
    int commonLevels = 0, skippedLevels = 0;
    int lastKeylen = 0;
    for(int i=0; i<keys.size(); i++)
    {

        if(i!= keys.size()-1)
        {
            if (isSameKey(keys[i],keys[i+1]))
            {
                dupCount++;
                continue;
            }

            commonLevels = computeCommonPrefix(keys[i],keys[i+1]);

            if(keys[i].length() == trie->cutOffLevel && commonLevels == trie->cutOffLevel)
            {
                peFlag = true;
            }
        }
        trie->insertKeyIntoTrie(keys[i],dupCount,peFlag);
        if(keys[i].length()> trie->cutOffLevel)
            louds->insertSet(keys[i],dupCount,skippedLevels, lastKeylen);
        lastKeylen = keys[i].length();
        dupCount=1;
        peFlag = false;
        skippedLevels =  commonLevels;
    }
}

int superSetQuery(Trie *trie, vector<Record> &querys,vector<ENIL> &res)
{
    int  i = 0;

    for(i=0; i<querys.size(); i++)
    {
        trie->superSetQuery(querys[i],res);
    }


    return 0;
}



int readData(vector<Record> &keys,char *dataFile)
{
    ifstream ifs(dataFile);
    if (!ifs.is_open())
        return -1;
    string s;
    int id = 0;
    map<int, int> freqMap;
    while(getline(ifs, s))
    {
        Record r(id++);

        istringstream iss(s);
        int num = 0;
        while (iss >> num)
        {
            r.AddNum(num);
            freqMap[num]++;
        }

        keys.push_back(r);
    }
    ifs.close();
    return 0;
}

void getQuerys(vector<Record>&keys,vector<Record> &querys,int num, char *queryFile)
{

    if(!isFileExist(queryFile))
    {
        FILE *fp = fopen(queryFile, "w");
        if(fp==NULL)
            return;
        int *ids = getNonRepeat(keys.size()-1,num);

        for(int i=0; i<num; i++)
        {
            Record r = keys[ids[i]];
            querys.push_back(r);
            for(int j=0; j<r.length()-1; j++)
            {
                fprintf(fp,"%d ",r[j]);
            }
            fprintf(fp,"%d\n",r[r.length()-1]);
        }
        fclose(fp);
    }
    else
    {
        readData(querys,queryFile);
    }

}
int main()
{
    char *dataFile ="/home/comp1/mycpp/TLI/retail_slexA_lexA";//dblp_le_fixed_slexA_freD
    char queryFile[200]= {""} ;
    int queryNum =20000;

    char *statFile ="/home/comp1/mycpp/TLI/result.txt";
    strcat(strcat(queryFile,dataFile),"_query");
    printf("test\n");

    resFp = fopen(statFile,"a+");
    if(resFp==NULL)
    {
        printf("open file error!");
        return 0;
    }

    vector<Record> keys, querys;
    if(readData(keys,dataFile)<0)
    {
        printf("read data error!\n");
        return 1;
    }
    else
    {
        printf("read data %d\n",keys.size());
    }

    getQuerys(keys,querys,queryNum,queryFile);

    printf("querys num:%d\n",querys.size());


    for(int c=0; c<8; c++)
    {
        fprintf(resFp,"\nfile:%s, cutOffLevel=%d\n",dataFile,cutOffLevel[c]);

        double totaltime;
        clock_t start=clock();

        Trie *trie = new Trie(cutOffLevel[c]);
        LOUDS_Level *louds_level= new LOUDS_Level(cutOffLevel[c]);
        createIndex(trie,louds_level,keys);

        LOUDS_Flat *louds_flat = new LOUDS_Flat(louds_level);


        trie->resFp = resFp;
        louds_flat->resFp =resFp;
        trie->setLOUDS(louds_flat); // since we need to traverse from trie to LOUDS, so we need to where LOUDS is in trie.

        louds_flat->setPNodeCount(trie->pNodeCount);

        delete louds_level;

        totaltime=(clock()-start)/double(CLOCKS_PER_SEC);
        printf("index construction time:[ %fs]\n", totaltime);
        printf("total trie & louds & total node:%d, %d, %d\n", trie->nodeCount, louds_flat->nodeCount,trie->nodeCount+louds_flat->nodeCount);
        printf("total ordinary trie node:%d\n", trie->nodeCount - trie->pNodeCount - trie->eNodeCount);
        printf("total trie pnode:%d\n", trie->pNodeCount);
        printf("total trie enode:%d\n", trie->eNodeCount);
        printf("total trie penode:%d\n", trie->peNodeCount);
        printf("total ENIL in LOUDS:%d\n", louds_flat->ENILCount);


        fprintf(resFp,"index construction time:[ %fs]\n",totaltime);
        fprintf(resFp,"total trie & louds & total node:%d, %d, %d\n", trie->nodeCount, louds_flat->nodeCount,trie->nodeCount+louds_flat->nodeCount);
        fprintf(resFp,"total ordinary trie node:%d\n", trie->nodeCount - trie->pNodeCount - trie->eNodeCount);
        fprintf(resFp,"total trie pnode:%d\n",trie->pNodeCount);
        fprintf(resFp,"total trie enode:%d\n",trie->eNodeCount);
        fprintf(resFp,"total trie penode:%d\n",trie->peNodeCount);
        fprintf(resFp,"total ENIL in LOUDS:%d\n", louds_flat->ENILCount);



        vector<ENIL> res;
        start = clock();

        superSetQuery(trie,querys,res);

        totaltime=(clock()-start)/double(CLOCKS_PER_SEC);

        printf("Super set query time:[ %fs]\n", totaltime);
        printf("trie found %d, louds found %d,total found %d\n",  trie->totalFound, louds_flat->totalFound, trie->totalFound + louds_flat->totalFound);
        printf("trie accessed %d, louds accessed %d, total accessed %d\n", trie->totalAccessed, louds_flat->totalAccessed, trie->totalAccessed + louds_flat->totalAccessed);


        fprintf(resFp,"Super set query time:[ %fs]\n", totaltime);
        fprintf(resFp,"trie found %d, louds found %d,total found %d\n",  trie->totalFound, louds_flat->totalFound, trie->totalFound + louds_flat->totalFound);
        fprintf(resFp,"trie accessed %d, louds accessed %d, total accessed %d\n", trie->totalAccessed, louds_flat->totalAccessed, trie->totalAccessed + louds_flat->totalAccessed);

        delete trie;

    }
    fclose(resFp);

    return 0;
}






