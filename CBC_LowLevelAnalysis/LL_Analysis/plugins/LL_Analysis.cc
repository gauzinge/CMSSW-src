// -*- C++ -*-
//
// Package:    LL_Analysis
// Class:      LL_Analysis
// 
/**\class LL_Analysis LL_Analysis.cc CBC_LowLevelAnalysis/LL_Analysis/plugins/LL_Analysis.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Georg Auzinger
//         Created:  Mon, 09 Dec 2013 15:25:43 GMT
// $Id$
//
//


// system include files
#include <memory>
#include <map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "Phase2TrackerDAQ/SiStripDigi/interface/SiStripCommissioningDigi.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1D.h"
#include "TH2D.h"


class LL_Analysis : public edm::EDAnalyzer {
   public:
      explicit LL_Analysis(const edm::ParameterSet&);
      ~LL_Analysis();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	  
	  std::vector<unsigned int> sensors;
	  
	  //Handle to Event
      edm::Handle<edm::DetSetVector<PixelDigi> >  cbcDigis_;
	  //Handle to Conditions Data for TDC histos
      edm::Handle<edm::DetSet<SiStripCommissioningDigi> >  cbcCommissioningEvent_;
	  
	  //Histograms
	// Convention
	// 50001 = dut_top
	// 50002 = dut_bottom
	// 50011 = fix_top
	// 50012 = fix_bottom
	  
	  
	  //Total number of Hits
	  std::map<unsigned int, TH1D*> h_hits;
      TH1D * h_hits_DUT_t;
      TH1D * h_hits_DUT_b;
      TH1D * h_hits_FIX_t;
      TH1D * h_hits_FIX_b;
	  
	  //TDC for every hit, by sensor
	  std::map<unsigned int, TH1D*> h_tdc;
      TH1D * h_tdc_dt;
      TH1D * h_tdc_db;
      TH1D * h_tdc_ft;
      TH1D * h_tdc_fb;
	  
	  //1 or 0 depending if hit or not
	  std::map<unsigned int, TH1D*> h_tot;
      TH1D * h_tot_dut_t;
      TH1D * h_tot_dut_b;
      TH1D * h_tot_fix_t;
      TH1D * h_tot_fix_b;     
	  
	  //Hit Distribution
	  // std::map<unsigned int, TH1D*> h_n_hits_A;
// 	  std::map<unsigned int, TH1D*> h_n_hits_B;
	  TH1D * h_n_hits_dut_A;
	  TH1D * h_n_hits_dut_B;
	  TH1D * h_n_hits_fix_A;
	  TH1D * h_n_hits_fix_B;
	  
      int n_events;
	  
	  
   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;	  
};


LL_Analysis::LL_Analysis(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
	sensors = iConfig.getUntrackedParameter< std::vector<unsigned int> >("sensors");
}


LL_Analysis::~LL_Analysis()
{

}



// ------------ method called for each event  ------------
void LL_Analysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;

  // helpers
  std::map<unsigned int, std::vector<int> > hits; 
  // std::map<unsigned int, int> n_hits; 
  int nhits_dut_A = 0, nhits_dut_B = 0, nhits_fix_A = 0, nhits_fix_B = 0;
  
	for (std::vector<unsigned int>::iterator sen_it = sensors.begin(); sen_it != sensors.end(); sen_it++)
	{
		std::stringstream sensor_id;
		sensor_id << *sen_it;
		// n_hits[*sen_it] = 0;
	}
   
	int tdc = 0;
	
	// | INFCBCB | INFCBCA | CNMCBCB | CNMCBCA
	
	//access conditions data for tdc phase of event
    iEvent.getByLabel("SiStripDigiCondDataproducer", "ConditionData", cbcCommissioningEvent_);
	
	DetSet<SiStripCommissioningDigi>::const_iterator it = cbcCommissioningEvent_->data.begin();
	for(; it != cbcCommissioningEvent_->data.end(); it++)
	{
		if (it->getKey() == 0x030000ff) // TDC Phase
		{
			int value = it->getValue();
			(value == 12) ? tdc = 0 : tdc = value - 4; // if not 12 -> value - 4
		}
	}
   
	//access Digis 
	iEvent.getByLabel("SiStripDigitestproducer", "ProcessedRaw", cbcDigis_);
   
	DetSetVector<PixelDigi>::const_iterator DSViter = cbcDigis_->begin();
	for (; DSViter!=cbcDigis_->end(); DSViter++) //module loop
	{
		DetSet<PixelDigi>::const_iterator DSiter = DSViter->data.begin();
		for(; DSiter != DSViter->data.end(); DSiter++) // hit loop
		{
			unsigned int detid = DSViter->id;
			int adc = DSiter->adc();
  		   
			if (adc > 250)
			{
				h_hits[detid]->Fill(DSiter->row());
  			   
				//Fill hit strips in vector per sensor
  	  		    hits[detid].push_back(DSiter->row());
				
				//Fill hit strips in vector per module / chip
				//DUT Chips A & B
				if ((detid == 51001 || detid == 51002) && DSiter->row() < 127) nhits_dut_A++;
				if ((detid == 51001 || detid == 51002) && DSiter->row() >= 127) nhits_dut_B++;
   
				//FIX Chips A & B
				if ((detid == 51011 || detid == 51012) && DSiter->row() < 127) nhits_fix_A++;
				if ((detid == 51011 || detid == 51012) && DSiter->row() >= 127) nhits_fix_B++;
			}
		} //End of hit loop
	} //End of module loop
   		 
	//Number of hits per chip for CM Noise
	h_n_hits_dut_A->Fill(nhits_dut_A);
	h_n_hits_dut_B->Fill(nhits_dut_B);
	h_n_hits_fix_A->Fill(nhits_fix_A);
	h_n_hits_fix_B->Fill(nhits_fix_B);
      	 	 
	for (std::vector<unsigned int>::iterator sen_it = sensors.begin(); sen_it != sensors.end(); sen_it++)
	{
	//Hit or no Hit
		h_tot[*sen_it]->Fill((hits[*sen_it].size()) ? 1 : 0);
	//Fill tdc for event
		if (hits[*sen_it].size()) h_tdc[*sen_it]->Fill(tdc);
	}
   
	LogDebug ("") << "hits dut A " << nhits_dut_A << " hits dut B " << nhits_dut_B << " hits fix A " << nhits_fix_A << " hits fix B " << nhits_fix_B ;
   
	n_events++;
}


// ------------ method called once each job just before starting event loop  ------------
void 
LL_Analysis::beginJob()
{
	n_events = 0;
	
	edm::Service<TFileService> fs;
	
	for (std::vector<unsigned int>::iterator sen_it = sensors.begin(); sen_it != sensors.end(); sen_it++)
	{
		std::stringstream sensor_id;
		sensor_id << *sen_it;
  	//Total number of Hits
		h_hits[*sen_it] = fs->make<TH1D>("h_hits_"+*sensor_id.str().c_str(),"Hits "+*sensor_id.str().c_str(),256,0.,256.);
	//Hit or no hit per sensor
		h_tot[*sen_it] = fs->make<TH1D>("h_tot_"+*sensor_id.str().c_str(),"Hits "+*sensor_id.str().c_str(),2,0.,2.);
	//TDC for every hit on every sensor  
	    h_tdc[*sen_it] = fs->make<TH1D>("h_tdc_"+*sensor_id.str().c_str(),"TDC "+*sensor_id.str().c_str(),10,0.,10.);
	  	//Hit Distribution
	  	// h_n_hits[*sen_id] = fs->make<TH1D>("h_n_hits_"+*sensor_id.str().c_str(),"Number of Hits DUT chip A", 256,0.,256.);
	}
	
  	//Hit Distribution
  	h_n_hits_dut_A = fs->make<TH1D>("h_n_hits_dut_A","Number of Hits DUT chip A", 256,0.,256.);
  	h_n_hits_dut_B = fs->make<TH1D>("h_n_hits_dut_B","Number of Hits DUT chip B", 256,0.,256.);
  	h_n_hits_fix_A = fs->make<TH1D>("h_n_hits_fix_A","Number of Hits FIX chip A", 256,0.,256.);
  	h_n_hits_fix_B = fs->make<TH1D>("h_n_hits_fix_B","Number of Hits FIX chip B", 256,0.,256.);
}


void 
LL_Analysis::endJob() 
{
}

void
LL_Analysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(LL_Analysis);
