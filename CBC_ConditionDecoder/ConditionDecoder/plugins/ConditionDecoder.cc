// -*- C++ -*-
//
// Package:    ConditionDecoder
// Class:      ConditionDecoder
// 
/**\class ConditionDecoder ConditionDecoder.cc CBC_ConditionDecoder/ConditionDecoder/plugins/ConditionDecoder.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Georg Auzinger
//         Created:  Mon, 09 Dec 2013 12:54:15 GMT
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Common/interface/Ref.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "Phase2TrackerDAQ/SiStripDigi/interface/SiStripCommissioningDigi.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "TH1D.h"

#define NSTUBS 0x0b0000ff
#define STUBMASK 0x00000003
#define TRGPHASE 0x030000ff
#define DUTANGLE 0x040000ff
#define HVDUT 0x05000000
#define HVFIX 0x05000001
#define CBCERROR 0x060000ff
#define PIPELINE 0x070000ff
#define TRGLATENCYDUT0 0x01014000 
#define TRGLATENCYDUT1 0x01014100
#define TRGLATENCYFIX0 0x01014001
#define TRGLATENCYFIX1 0x01014101
#define VCTHDUT0 0x017F4000
#define VCTHDUT1 0x017F4100
#define VCTHFIX0 0x017F4001
#define VCTHFIX1 0x017F4101


class ConditionDecoder : public edm::EDAnalyzer {
   public:
      explicit ConditionDecoder(const edm::ParameterSet&);
      ~ConditionDecoder();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

      edm::Handle<edm::DetSet<SiStripCommissioningDigi> >  cbcCommissioningEvent_;

	  //Histograms
      TH1D * h_pipeline;
      TH1D * h_pipematch;
      TH1D * h_error;
      TH1D * h_tdc;
      TH1D * h_stubs_FIX;
      TH1D * h_stubs_DUT;
 
      TH1D * h_stub_tdc_dut;
      TH1D * h_stub_tdc_fix;
      TH1D * h_stub_dut;
      TH1D * h_stub_fix;
	  
	  TH1D * h_angle_dut;
	  TH1D * h_hv_dut;
	  TH1D * h_hv_fix;
	  
	  TH1D* h_trg_latency_dut;
	  TH1D* h_trg_latency_fix;
	  
	  TH1D* h_vcth_dut;
	  TH1D* h_vcth_fix;
 
	  
   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      int n_events;
};


ConditionDecoder::ConditionDecoder(const edm::ParameterSet& iConfig)

{
	//eventually initialize variables for data.... in case i want to do more
}


ConditionDecoder::~ConditionDecoder()
{
 
}


// ------------ method called for each event  ------------
void
ConditionDecoder::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   iEvent.getByLabel("SiStripDigiCondDataproducer", "ConditionData", cbcCommissioningEvent_);
	
   unsigned int stub_dut = 0;
   unsigned int stub_fix = 0;
   int tdc = 0;
   
   // | INFCBCB | INFCBCA | CNMCBCB | CNMCBCA
   
   //loop over CommissioningDigi
   DetSet<SiStripCommissioningDigi>::const_iterator it = cbcCommissioningEvent_->data.begin();
   for(; it != cbcCommissioningEvent_->data.end(); it++)
   {
	   if( it->getKey() == NSTUBS ) //Stubs
	   {
		   int value = it->getValue();
		   int stubs_DUT = value & STUBMASK; //decode stub bits DUT & FIX
		   int stubs_FIX = (value >> 2 ) & STUBMASK;
		   
		   //Fill Stub Histograms
		   h_stubs_DUT->Fill(stubs_DUT);
		   h_stubs_FIX->Fill(stubs_FIX);
		   
		   //Flags for later use
		   if (stubs_DUT) stub_dut = 1;
		   if (stubs_FIX) stub_fix = 1;
	   }
	   
	   if (it->getKey() == TRGPHASE) // TDC Phase
	   {
		   int value = it->getValue();
		   (value == 12) ? tdc = 0 : tdc = value - 4; // if not 12 -> value - 4
           h_tdc->Fill(value);
	   }
	   
	   if (it->getKey() == DUTANGLE) // DUT Angle or similar
	   {
		   int angle = it->getValue();
		   h_angle_dut->Fill(angle);
	   }
	   
	   //This could be done more elegant
	   if (it->getKey() == HVDUT) //HV Settings DUT
	   {
		   int hv_DUT = it->getValue(); 
		   h_hv_dut->Fill(hv_DUT);		   
	   }
	   if (it->getKey() == HVFIX) //HV Settings FIX
	   {
		   int hv_FIX = it->getValue(); 
		   h_hv_fix->Fill(hv_FIX);
	   }
	   
       if( it->getKey() == CBCERROR ) //CBC Error
	   {
         	int value = it->getValue() & 0x0000000f;
        	h_error->Fill(value);
       }
	   
       if( it->getKey() == PIPELINE ) //Pipeline Address
	   {
		   int value = it->getValue();
		   int pipe_addr[4]={0,0,0,0};
		   
		   for (int i = 0; i < 4; i++)
		   {
			   pipe_addr[i] = value >> (i*8) & 0xff; //shift right by i*8 bit
		   }
		   
		   h_pipeline->Fill(pipe_addr[0]);
		   int pipematch = ((pipe_addr[0] ^ pipe_addr[1]) ^ (pipe_addr[2] ^ pipe_addr[3])) ? 1 : 0 ;
		   h_pipematch->Fill(pipematch);
	   }
	   
	   if (it->getKey() == TRGLATENCYDUT0 || it->getKey() == TRGLATENCYDUT1 ) // TRG Latency DUT (fill both in same histogram and check at readtime for multiple bins)
	   {
		   h_trg_latency_dut->Fill(static_cast<double>(it->getValue()));
	   }
	   if (it->getKey() == TRGLATENCYFIX0 || it->getKey() == TRGLATENCYFIX1 ) // TRG Latency FIX (fill both in same histogram and check at readtime for multiple bins)
	   {
		   h_trg_latency_fix->Fill(static_cast<double>(it->getValue()));
	   }
	   
	   if (it->getKey() == VCTHDUT0 || it->getKey() == VCTHDUT1 ) // VCTH for DUT fill both in same histo and check for multiple bins at readtime
	   {
		   h_vcth_dut->Fill(static_cast<double>(it->getValue()));
	   }
	   if (it->getKey() == VCTHFIX0 || it->getKey() == VCTHFIX1 ) // VCTH for FIX
	   {
		   h_vcth_fix->Fill(static_cast<double>(it->getValue()));
	   }
   }
   
   //Increment Event Counter
   n_events++;
   
   //Fill some additional Histograms
   if (stub_dut) h_stub_tdc_dut->Fill(tdc);
   if (stub_fix) h_stub_tdc_fix->Fill(tdc);
   
   h_stub_dut->Fill(stub_dut);
   h_stub_fix->Fill(stub_fix);
}


// ------------ method called once each job just before starting event loop  ------------
void 
ConditionDecoder::beginJob()
{
    n_events = 0;
	
    edm::Service<TFileService> fs;
	
	//Stubs
    h_stubs_DUT = fs->make<TH1D>("h_stubs_DUT","Stubs DUT (0,A,B,both)",5,0.,5.);
    h_stubs_FIX = fs->make<TH1D>("h_stubs_FIX","Stubs FIX (0,A,B,both)",5,0.,5.);
	
    h_stub_tdc_dut = fs->make<TH1D>("h_stub_tdc_d","Stubs TDC DUT",10,0.,10.);
    h_stub_tdc_fix = fs->make<TH1D>("h_stub_tdc_f","Stubs TDC FIX",10,0.,10.);

    h_stub_dut = fs->make<TH1D>("h_stub_dut","Stubs DUT yes/no",2,0.,2.);
    h_stub_fix = fs->make<TH1D>("h_stub_fix","Stubs FIX yes/no",2,0.,2.);
	
	//TDC
    h_tdc = fs->make<TH1D>("h_tdc","tdc",256,0.,256.);
	
	//Pipeline Address
    h_pipeline = fs->make<TH1D>("h_pipeline","pipeline",256,0.,256.);
    h_pipematch = fs->make<TH1D>("h_pipematch","pipematch",2,0.,2.);
	
	//CBC Errors
    h_error = fs->make<TH1D>("h_error","error",16,0.,16.);	
	
	//Angle
	h_angle_dut = fs->make<TH1D>("h_angle_dut","DUT Rotation Angle",20,-0.5,19.5);
	
	//HV Settings
	h_hv_dut = fs->make<TH1D>("h_hv_dut","DUT HV Settings",500,0.,500.);
	h_hv_fix = fs->make<TH1D>("h_hv_fix","FIX HV Settings",500,0.,500.);
	
	// TRG Latency
	h_trg_latency_dut = fs->make<TH1D>("h_trg_latency_dut","DUT TRG Latency",35,0.,35.);
	h_trg_latency_fix = fs->make<TH1D>("h_trg_latency_fix","FIX TRG Latency",35,0.,35.);
	
	//VCth
	h_vcth_dut = fs->make<TH1D>("h_vcth_dut","DUT Comparator Threshold",250,0.,250.);
	h_vcth_fix = fs->make<TH1D>("h_vcth_fix","FIX Comparator Threshold",250,0.,250.);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ConditionDecoder::endJob() 
{
}

void
ConditionDecoder::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ConditionDecoder);
