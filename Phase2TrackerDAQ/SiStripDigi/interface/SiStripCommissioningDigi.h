#ifndef AnalysisDataFormats_SiStripDigi_SiStripCommissioningDigi_H
#define AnalysisDataFormats_SiStripDigi_SiStripCommissioningDigi_H

//#include <map> 
//typedef std::map<uint32_t, uint32_t> SiStripCommissioningDigi; 

class SiStripCommissioningDigi
{
  public:
    SiStripCommissioningDigi():key_(0),value_(0) {}
    SiStripCommissioningDigi(uint32_t key, uint32_t value):key_(key),value_(value) {}
    ~SiStripCommissioningDigi() {}
    uint32_t getKey() const { return key_; }
    uint32_t getValue() const { return value_; }
  private:
    uint32_t key_;
    uint32_t value_; 
};

#endif
