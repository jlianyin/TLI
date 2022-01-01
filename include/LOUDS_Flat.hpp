#ifndef LOUDS_FLAT_INCLUDED
#define LOUDS_FLAT_INCLUDED

#include <string>

#include "config.hpp"
#include "label_vector.hpp"
#include "rank.hpp"
#include "select.hpp"
#include "LOUDS_Level.hpp"

namespace TLI
{
class LOUDS_Flat
{
public:

    LOUDS_Flat(LOUDS_Level* louds_l);
    ~LOUDS_Flat() {
    labels_->destroy();
	child_indicator_bits_->destroy();
	louds_bits_->destroy();
	enil_bits_->destroy();
    }

    void lowerSearch(Record &query,int level,int pNodeNum, vector<ENIL> &res);
    void setPNodeCount(int c)
    {
        pNodeCount = c;
    }

    position_t getRankNo(BitvectorRank* bvr, const position_t pos) const ;
    position_t getSelectPos(BitvectorSelect* bvs,const position_t node_num) const;
    position_t nodeSize(const position_t pos) const;
    LabelVector* labels_;
    BitvectorRank* child_indicator_bits_;
    BitvectorSelect* louds_bits_;
    BitvectorRank* enil_bits_;
    int cutOffLevel;
    int pNodeCount;

    int nodeCount;
    int ENILCount;

    uint32_t totalFound;
    uint32_t totalAccessed;

    FILE *resFp;


private:
    static const position_t kRankBasicBlockSize = 512;
    static const position_t kSelectSampleInterval = 64;
    level_t height_; // trie height
    level_t start_level_;

    std::vector<ENIL> ENILS;

};


inline LOUDS_Flat::LOUDS_Flat(LOUDS_Level* louds_l)
{
    nodeCount = 0;
    height_ = louds_l->getLabels().size();

    start_level_ = 0;
    ENILCount = 0;

    labels_ = new LabelVector(louds_l->getLabels(), start_level_, height_);

    std::vector<position_t> num_items_per_level;
    for (level_t level = 0; level < height_; level++)
    {
        num_items_per_level.push_back(louds_l->getLabels()[level].size());
        nodeCount +=louds_l->getLabels()[level].size();
    }


    child_indicator_bits_ = new BitvectorRank(kRankBasicBlockSize, louds_l->getChildIndicatorBits(),
            num_items_per_level, start_level_, height_);
    louds_bits_ = new BitvectorSelect(kSelectSampleInterval, louds_l->getLoudsBits(),
                                      num_items_per_level, start_level_, height_);
    enil_bits_ = new BitvectorRank(kRankBasicBlockSize, louds_l->getENILBits(),
                                   num_items_per_level, start_level_, height_);

    for (level_t level = start_level_; level < height_; level++)
    {
       for (position_t idx = 0; idx < louds_l->ENILS[level].size(); idx++)
       {
       ENILS.push_back(louds_l->ENILS[level][idx]);

       }
       ENILCount+=louds_l->ENILS[level].size();
    }


    totalFound =0;
    totalAccessed=0;

}



inline position_t LOUDS_Flat::nodeSize(const position_t pos) const
{
    assert(louds_bits_->readBit(pos));
    return louds_bits_->distanceToNextSetBit(pos);
}

inline position_t LOUDS_Flat::getSelectPos(BitvectorSelect* bvs,const position_t node_num) const
{
    return bvs->select(node_num+1);
}


inline position_t LOUDS_Flat::getRankNo(BitvectorRank* bvr, const position_t pos) const {
    return (bvr->rank(pos) );
}

//louds->lowerSerch(query,level+1,pNodeNum,res);
void LOUDS_Flat::lowerSearch(Record &query,int level,int pNodeNum, vector<ENIL> &res)
{
    while(level < query.length())
    {
        position_t pos = getSelectPos(louds_bits_, pNodeNum);
        bool find=labels_->binarySearch((label_t)query[level], pos, nodeSize(pos));
        totalAccessed += 1;
        if(find)
        {
            if(enil_bits_->readBit(pos))
            {

                position_t enilPos = getRankNo(enil_bits_,pos);
                res.push_back(ENILS[enilPos-1]);
                totalFound += ENILS[enilPos-1].dupCount;
                //fprintf(resFp,"%d, %d\n",ENILS[enilPos-1].startID,ENILS[enilPos-1].dupCount);
            }

            if (child_indicator_bits_->readBit(pos))
            {
                //pNodeNum = pNodeCount + getRankNo(child_indicator_bits_,pos);

                lowerSearch(query,level+1,max(pNodeCount-1,0) + getRankNo(child_indicator_bits_,pos),res);
            }


        }
        level++;
    }

}
}

#endif // LOUDS_FLAT_INCLUDED
