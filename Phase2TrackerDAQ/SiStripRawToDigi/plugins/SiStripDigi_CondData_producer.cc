#include "Phase2TrackerDAQ/SiStripRawToDigi/plugins/SiStripDigi_CondData_producer.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/src/fed_header.h"
#include "DataFormats/FEDRawData/src/fed_trailer.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripCommon/interface/SiStripEventSummary.h"
#include "EventFilter/SiStripRawToDigi/interface/TFHeaderDescription.h"
#include "Phase2TrackerDAQ/SiStripRawToDigi/interface/SiStripFEDBuffer_CBC.h"
#include "CondFormats/SiStripObjects/interface/SiStripFedCabling.h"
#include "CondFormats/DataRecord/interface/SiStripFedCablingRcd.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ext/algorithm>

using namespace std;

namespace sistrip {

  SiStripDigi_CondData_producer::SiStripDigi_CondData_producer( const edm::ParameterSet& pset ) :
    runNumber_(0),
    productLabel_(pset.getParameter<edm::InputTag>("ProductLabel")),
    cabling_(0),
    cacheId_(0)
  {
    produces< edm::DetSet<SiStripCommissioningDigi> >("ConditionData");
  }
  
  SiStripDigi_CondData_producer::~SiStripDigi_CondData_producer()
  {
  }
  
  void SiStripDigi_CondData_producer::beginJob( const edm::EventSetup & )
  {
  }
  
  void SiStripDigi_CondData_producer::beginRun( edm::Run & run, const edm::EventSetup & es)
  {
  }
  
  void SiStripDigi_CondData_producer::endJob()
  {
  }
  
  void SiStripDigi_CondData_producer::produce( edm::Event& event, const edm::EventSetup& es)
  {
    // Retrieve FEDRawData collection
    edm::Handle<FEDRawDataCollection> buffers;
    event.getByLabel( "rawDataCollector", buffers );

    // Analyze strip tracker FED buffers in data
    size_t fedIndex;
    for( fedIndex=0; fedIndex<sistrip::CMS_FED_ID_MAX; ++fedIndex )
    {
      const FEDRawData& fed = buffers->FEDData(fedIndex);
      if(fed.size()!=0 && fedIndex >= sistrip::FED_ID_MIN && fedIndex <= sistrip::FED_ID_MAX)
      {
	// construct buffer
	sistrip:: FEDBuffer_CBC* buffer = 0;
	buffer = new sistrip::FEDBuffer_CBC(fed.data(),fed.size());

        // fetch condition data
        std::map<uint32_t,uint32_t> cond_data = buffer->conditionData();
        delete buffer;

        // print cond data for debug
        LogTrace("SiStripDigi_CondData_producer") << "--- Condition data debug ---" << std::endl;
        std::map<uint32_t,uint32_t>::const_iterator it;
        for(it = cond_data.begin(); it != cond_data.end(); it++)
        {
          LogTrace("SiStripDigi_CondData_producer") << std::hex << "key: " << it->first 
                                                    << std::hex << " value: " << it->second << " (hex) "  
                                                    << std::dec               << it->second << " (dec) " << std::endl;
        }
        LogTrace("SiStripDigi_CondData_producer") << "----------------------------" << std::endl;
        // store it into digis
        edm::DetSet<SiStripCommissioningDigi> *cond_data_digi = new edm::DetSet<SiStripCommissioningDigi>(fedIndex);
        for(it = cond_data.begin(); it != cond_data.end(); it++) 
        {
          cond_data_digi->push_back(SiStripCommissioningDigi(it->first,it->second));
        }
        std::auto_ptr< edm::DetSet<SiStripCommissioningDigi> > cdd(cond_data_digi);
        event.put( cdd, "ConditionData" );
      }
    }
  }
}
