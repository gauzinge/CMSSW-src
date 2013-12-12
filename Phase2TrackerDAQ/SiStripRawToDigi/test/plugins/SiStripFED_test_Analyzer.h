#ifndef EventFilter_SiStripRawToDigi_SiStripFED_test_Analyzer_H
#define EventFilter_SiStripRawToDigi_SiStripFED_test_Analyzer_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <utility>
#include <vector>

/**
   @class SiStripFED_test_Analyzer 
   @brief Analyzes contents of FED_test_ collection
*/

class SiStripFED_test_Analyzer : public edm::EDAnalyzer {
  
 public:
  
  typedef std::pair<uint16_t,uint16_t> Fed;
  typedef std::vector<Fed> Feds;
  typedef std::vector<uint16_t> Channels;
  typedef std::map<uint16_t,Channels> ChannelsMap;

  SiStripFED_test_Analyzer( const edm::ParameterSet& );
  ~SiStripFED_test_Analyzer();

  void beginJob();
  void analyze( const edm::Event&, const edm::EventSetup& );
  void endJob();

 private:

  edm::InputTag label_;
};

#endif // EventFilter_SiStripRawToDigi_SiStripFED_test_Analyzer_H

