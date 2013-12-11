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
#include "DataFormats/DetId/interface/DetId.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1D.h"
#include "TH1I.h"
#include "TH2D.h"


class LL_Analysis : public edm::EDAnalyzer {
   public:
      explicit LL_Analysis(const edm::ParameterSet&);
      ~LL_Analysis();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	  
	  // int get_chip(int strip_no);
	  
	  //Handles to Event
      edm::Handle<edm::DetSetVector<PixelDigi> >  cbcDigis_;
	  
	  //Histograms
	  
	  //Total number of Hits
      TH1D * h_hits_DUT_t;
      TH1D * h_hits_DUT_b;
      TH1D * h_hits_FIX_t;
      TH1D * h_hits_FIX_b;
	  
      TH1D * h_tot_dut_t;
      TH1D * h_tot_dut_b;
      TH1D * h_tot_fix_t;
      TH1D * h_tot_fix_b;     
	  
	  //Hit Distribution
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
}


LL_Analysis::~LL_Analysis()
{

}

// int LL_Analysis::get_chip(int strip_no)
// {
// 	if (strip_no < 127) return 0;
// 	else return 1;
// }


// ------------ method called for each event  ------------
void LL_Analysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;

	iEvent.getByLabel("SiStripDigitestproducer", "ProcessedRaw", cbcDigis_);

	std::vector<int> hits_db;
	std::vector<int> hits_dt;
	std::vector<int> hits_ft;
	std::vector<int> hits_fb;
   
	int nhits_dut_A = 0;
	int nhits_dut_B = 0;
	int nhits_fix_A = 0;
	int nhits_fix_B = 0;
   
   
	// | INFCBCB | INFCBCA | CNMCBCB | CNMCBCA
   
	DetSetVector<PixelDigi>::const_iterator DSViter = cbcDigis_->begin();
	for (; DSViter!=cbcDigis_->end(); DSViter++) //module loop
	{
		DetSet<PixelDigi>::const_iterator DSiter = DSViter->data.begin();
		for(; DSiter != DSViter->data.end(); DSiter++) // hit loop
		{
			int detid = DSViter->id;
			int adc = DSiter->adc();
  		   
			if (adc > 250)
			{
				//Fill strip number of every hit in Histo - > Beam Profile
				switch (detid)
				{
					case 51001: h_hits_DUT_t->Fill(DSiter->row()); break; //CNM top
					case 51002: h_hits_DUT_b->Fill(DSiter->row()); break; //CNM bottom
					case 51011: h_hits_FIX_t->Fill(DSiter->row()); break; //Infineon top
					case 51012: h_hits_FIX_b->Fill(DSiter->row()); break; //Infineon bottom
				}
  			   
				//Fill hit strips in vector per sensor
				if (detid == 51001) hits_dt.push_back(DSiter->row());
				if (detid == 51002) hits_db.push_back(DSiter->row());
				if (detid == 51011) hits_ft.push_back(DSiter->row());
				if (detid == 51012) hits_fb.push_back(DSiter->row());
  	  
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
   		 
	h_n_hits_dut_A->Fill(nhits_dut_A);
	h_n_hits_dut_B->Fill(nhits_dut_B);
	h_n_hits_fix_A->Fill(nhits_fix_A);
	h_n_hits_fix_B->Fill(nhits_fix_B);
      	 	 
	//Fill with 0/1 
	h_tot_dut_t->Fill((hits_dt.size()) ? 1 : 0);
	h_tot_dut_b->Fill((hits_db.size()) ? 1 : 0);
	h_tot_fix_t->Fill((hits_ft.size()) ? 1 : 0);
	h_tot_fix_b->Fill((hits_fb.size()) ? 1 : 0);
   
	LogDebug ("") << "hits dut A " << nhits_dut_A << " hits dut B " << nhits_dut_B << " hits fix A " << nhits_fix_A << " hits fix B " << nhits_fix_B ;
   
	n_events++;
}


// ------------ method called once each job just before starting event loop  ------------
void 
LL_Analysis::beginJob()
{
	n_events = 0;
	
	edm::Service<TFileService> fs;
	
  	//Total number of Hits
    h_hits_DUT_b = fs->make<TH1D>("h_hits_DUT_b","Hits DUT top",256,0.,256.);
    h_hits_DUT_t = fs->make<TH1D>("h_hits_DUT_t","Hits DUT bot",256,0.,256.);
    h_hits_FIX_b = fs->make<TH1D>("h_hits_FIX_b","Hits FIX top",256,0.,256.);
    h_hits_FIX_t = fs->make<TH1D>("h_hits_FIX_t","Hits FIX bot",256,0.,256.);
	
  	h_tot_dut_t = fs->make<TH1D>("h_tot_dut_t","Hits DUT top",256,0.,256.);
  	h_tot_dut_b = fs->make<TH1D>("h_tot_dut_b","Hits DUT bot",256,0.,256.);
  	h_tot_fix_t = fs->make<TH1D>("h_tot_fix_t","Hits FIX top",256,0.,256.);
  	h_tot_fix_b = fs->make<TH1D>("h_tot_fix_b","Hits FIX bot",256,0.,256.);     

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
