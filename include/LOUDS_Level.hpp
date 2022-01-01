#ifndef LOUDS_HPP_INCLUDED
#define LOUDS_HPP_INCLUDED

#include <assert.h>
#include <vector>
#include "config.hpp"
#include "Record.hpp"
namespace TLI
{
class LOUDS_Level
{
public:
    LOUDS_Level(int level)
    {
        cutOffLevel =level;
    }

    int insertSet(Record &key, int dupCount, level_t skippedLevel, int lastKeyLen);
    void insertOneElement(int dupCount,const Record& key, const level_t level, const bool isFirstChild);
    inline void moveToNextItemSlot(const level_t level);
    inline level_t getLOUDSHeight() const ;
    inline position_t getNumItems(const level_t level) const;
    inline bool isLevelEmpty(const level_t level) const;
    inline void addLevel();
    void flatIndex();
    const std::vector<std::vector<label_t> >& getLabels() const
    {
        return labels_;
    }
    const std::vector<std::vector<word_t> >& getChildIndicatorBits() const
    {
        return child_indicator_bits_;
    }
    const std::vector<std::vector<word_t> >& getLoudsBits() const
    {
        return louds_bits_;
    }
    const std::vector<std::vector<word_t> >& getENILBits() const
    {
        return ENIL_bits_;
    }

    static uint32_t findNumberofOnes(uint32_t n)
    {
        n = (n & 0x55555555)+ ((n>>1) & 0x55555555);
        n = (n & 0x33333333)+ ((n>>2) & 0x33333333);
        n = (n & 0x0f0f0f0f)+ ((n>>4) & 0x0f0f0f0f);
        n = (n & 0x00ff00ff)+ ((n>>8) & 0x00ff00ff);
        n = (n & 0x0000ffff)+ ((n>>16) & 0x0000ffff);
        return n;
    }
    static void setBit(std::vector<word_t>& bits, const position_t pos)
    {
        assert(pos < (bits.size() * kWordSize));
        position_t word_id = pos / kWordSize;
        position_t offset = pos % kWordSize;
        bits[word_id] |= (kMsbMask >> offset);
    }

    static bool readBit(const std::vector<word_t>& bits, const position_t pos)
    {
        assert(pos < (bits.size() * kWordSize));
        position_t word_id = pos / kWordSize;
        position_t offset = pos % kWordSize;
        return (bits[word_id] & (kMsbMask >> offset));
    }
    int cutOffLevel;
    int pNodeCount;
    std::vector<std::vector<label_t> > labels_;
    std::vector<std::vector<word_t> > child_indicator_bits_;
    std::vector<std::vector<word_t> > louds_bits_;
    std::vector<std::vector<word_t> > ENIL_bits_;
    std::vector<std::vector<ENIL> > ENILS;
    FILE* resFp;

};

inline level_t LOUDS_Level:: getLOUDSHeight() const
{
    return labels_.size();
}

inline position_t LOUDS_Level::getNumItems(const level_t levelinLOUDS) const
{
    return labels_[levelinLOUDS].size();
}

inline bool LOUDS_Level::isLevelEmpty(const level_t levelinLOUDS) const
{
    return (levelinLOUDS >= getLOUDSHeight()) || (labels_[levelinLOUDS].size() == 0);
}

inline void LOUDS_Level::moveToNextItemSlot(const level_t levelinLOUDS)
{
    //assert(level < getLOUDSHeight());
    position_t num_items = getNumItems(levelinLOUDS);
    if (num_items % kWordSize == 0)
    {
        child_indicator_bits_[levelinLOUDS].push_back(0);
        louds_bits_[levelinLOUDS].push_back(0);
        ENIL_bits_[levelinLOUDS].push_back(0);
    }
}

inline void LOUDS_Level::addLevel()
{
    labels_.push_back(std::vector<label_t>());
    child_indicator_bits_.push_back(std::vector<word_t>());
    louds_bits_.push_back(std::vector<word_t>());
    ENIL_bits_.push_back(std::vector<word_t>());
    ENILS.push_back(std::vector<ENIL>());
    child_indicator_bits_[getLOUDSHeight() - 1].push_back(0);
    louds_bits_[getLOUDSHeight() - 1].push_back(0);
    ENIL_bits_[getLOUDSHeight() - 1].push_back(0);

}

void LOUDS_Level::insertOneElement(int dupCount,const Record& key, const level_t level, const bool isFirstChild)
{

    level_t levelinLOUDS = level - cutOffLevel;
    if (level - cutOffLevel >= getLOUDSHeight())
        addLevel();
    assert(level- cutOffLevel < getLOUDSHeight());

    if (level>cutOffLevel)
    {
        setBit(child_indicator_bits_[levelinLOUDS-1], getNumItems(levelinLOUDS-1) - 1);
    }


    labels_[levelinLOUDS].push_back(key[level]);

    if (isFirstChild)
    {
        setBit(louds_bits_[levelinLOUDS], getNumItems(levelinLOUDS) - 1);
    }

    if(level==key.length()-1)
    {
        setBit(ENIL_bits_[levelinLOUDS], getNumItems(levelinLOUDS) - 1);
        ENILS[levelinLOUDS].push_back(ENIL(key.id_-dupCount + 1,dupCount));
    }

    moveToNextItemSlot(levelinLOUDS);//用于分段

}

int LOUDS_Level::insertSet(Record &key,int dupCount, level_t skippedLevel, int lastKeyLen)
{

    level_t level = cutOffLevel>skippedLevel?cutOffLevel : skippedLevel;
    bool isFirstChild =false;
    // If it is the start of level, the louds bit needs to be set.

    if (isLevelEmpty(level-cutOffLevel))
        isFirstChild = true;
    else if(level-skippedLevel>=1) //level starts from 0, while skippedLevel from 1
        isFirstChild = true;
    else if(level+1 > lastKeyLen)
        isFirstChild = true;


    insertOneElement(dupCount,key, level, isFirstChild);
    level++;

//跳过公共前缀之后的第一个要插入的节点需要判断是不是startnode,接下来的每一个要插入的节嗲都是startnode
    isFirstChild =true;
    while (level < key.length())
    {
        insertOneElement(dupCount,key, level, isFirstChild);
        level++;
    }

//    return level;

    return 0;
}

}
#endif
