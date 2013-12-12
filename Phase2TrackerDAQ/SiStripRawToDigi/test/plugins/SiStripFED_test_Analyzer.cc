#include "CondFormats/DataRecord/interface/SiStripFedCablingRcd.h"
#include "CondFormats/SiStripObjects/interface/SiStripFedCabling.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripDigi/interface/SiStripDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"
#include "Phase2TrackerDAQ/SiStripRawToDigi/test/plugins/SiStripFED_test_Analyzer.h"
#include "Phase2TrackerDAQ/SiStripRawToDigi/interface/SiStripFEDBuffer_CBC.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <sstream>
#include <string>
#include <map>

using namespace sistrip;
using namespace std;

// -----------------------------------------------------------------------------
// 
SiStripFED_test_Analyzer::SiStripFED_test_Analyzer( const edm::ParameterSet& pset ) : 
  label_(pset.getParameter<edm::InputTag>("InputLabel"))
{
  LogDebug("SiStripFED_test_Analyzer")
    << "[SiStripFED_test_Analyzer::"
    << __func__ << "]"
    << "Constructing object...";
}

// -----------------------------------------------------------------------------
// 
SiStripFED_test_Analyzer::~SiStripFED_test_Analyzer() 
{
  LogDebug("SiStripFED_test_Analyzer")
    << "[SiStripFED_test_Analyzer::"
    << __func__ << "]"
    << " Destructing object...";
}

// -----------------------------------------------------------------------------
// 
void SiStripFED_test_Analyzer::beginJob() 
{
  LogDebug("SiStripFED_test_Analyzer")
    << "[SiStripFED_test_Analyzer::"
    << __func__ << "]";
}

// -----------------------------------------------------------------------------
// 
void SiStripFED_test_Analyzer::endJob() 
{
  LogDebug("SiStripFED_test_Analyzer")
    << "[SiStripFED_test_Analyzer::"
    << __func__ << "]";
}

// -----------------------------------------------------------------------------
// 
void SiStripFED_test_Analyzer::analyze( const edm::Event& event, const edm::EventSetup& setup ) 
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

      cout << " -------------------------------------------- " << endl;
      cout << " buffer debug ------------------------------- " << endl;
      cout << " -------------------------------------------- " << endl;
      cout << " buffer size : " << buffer->bufferSize() << endl;
      cout << " fed id      : " << fedIndex << endl;
      cout << " -------------------------------------------- " << endl;
      cout << " tracker header debug ------------------------" << endl;
      cout << " -------------------------------------------- " << endl;
      
      TrackerHeader_CBC tr_header = buffer->trackerHeader();
      cout << " Version  : " << hex << setw(2) << (int) tr_header.getDataFormatVersion() << endl;
      cout << " Mode     : " << hex << setw(2) << (int) tr_header.getDebugMode() << endl;
      cout << " Type     : " << hex << setw(2) << (int) tr_header.getEventType() << endl;
      cout << " Readout  : " << hex << setw(2) << (int) tr_header.getReadoutMode() << endl;
      cout << " Status   : " << hex << setw(16)<< (int) tr_header.getGlibStatusCode() << endl;
      cout << " FE stat  : " ;
      for(int i=15; i>=0; i--)
      {
        if((tr_header.frontendStatus())[i])
        {
          cout << "1";
        }
        else
        { 
          cout << "0";
        }
      } 
      cout << endl;
      cout << " Nr CBC   : " << hex << setw(16)<< (int) tr_header.getNumberOfCBC() << endl;
      cout << " CBC stat : ";
      for(int i=0; i<tr_header.getNumberOfCBC(); i++)
      {
        cout << hex << setw(2) << (int) tr_header.CBCStatus()[i] << " ";
      }
      cout << endl;
      cout << " -------------------------------------------- " << endl;
      cout << " Payload  ----------------------------------- " << endl;
      cout << " -------------------------------------------- " << endl;

      // create digis containers
      std::vector<SiStripRawDigi> strips;

      // loop channels
      int ichan = 0;
      for ( int ife = 0; ife < 16; ife++ ) 
      {
        for ( int icbc = 0; icbc < 16; icbc++ )
        {
          const FEDChannel_CBC& channel = buffer->channel(ichan);
          if(channel.length() > 0) 
          {
            cout << dec << " reading channel : " << icbc << " on FE " << ife;
            cout << dec << " with length  : " << (int) channel.length() << endl;
            FEDRawChannelUnpacker_CBC unpacker = FEDRawChannelUnpacker_CBC(channel);
            while (unpacker.hasData())
            {
              strips.push_back(SiStripRawDigi( static_cast<uint16_t>(unpacker.stripOn()) ));
              if(unpacker.stripOn())
              { std::cout << "1";
              } else
              { std::cout << "_";
              }
              unpacker++;
            }
            std::cout << std::endl;
          }
          ichan ++;
        }
      } // end loop on channels
    }
  }
}
