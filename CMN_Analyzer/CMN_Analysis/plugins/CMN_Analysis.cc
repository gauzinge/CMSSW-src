// -*- C++ -*-
//
// Package:    CMN_Analysis
// Class:      CMN_Analysis
// 
/**\class CMN_Analysis CMN_Analysis.cc CMN_Analyzer/CMN_Analysis/plugins/CMN_Analysis.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Georg Auzinger
//         Created:  Wed, 02 Apr 2014 14:18:28 GMT
// $Id$
//
//


// system include files
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

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
#include "TProfile.h"
#include "TProfile2D.h"

//
// class declaration
//

class CMN_Analysis : public edm::EDAnalyzer {
public:
	explicit CMN_Analysis(const edm::ParameterSet&);
	~CMN_Analysis();
	
	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	
	//Handle to Event
	edm::Handle<edm::DetSetVector<PixelDigi> >  cbcDigis_;
	//Handle to Conditions Data for TDC histos
	edm::Handle<edm::DetSet<SiStripCommissioningDigi> >  cbcCommissioningEvent_;
	  
	//Hit Distribution per Chip / Sensor
	TH1D* h_n_hits_dut_t_A;
	TH1D* h_n_hits_dut_b_A;
	TH1D* h_n_hits_dut_t_B;
	TH1D* h_n_hits_dut_b_B;
	TH1D* h_n_hits_fix_t_A;
	TH1D* h_n_hits_fix_b_A;
	TH1D* h_n_hits_fix_t_B;
	TH1D* h_n_hits_fix_b_B;
	  
	//Hit Distribution per Chip
	TH1D* h_n_hits_dut_A;
	TH1D* h_n_hits_dut_B;
	TH1D* h_n_hits_fix_A;
	TH1D* h_n_hits_fix_B;
	   
	TH2D* h_cmn_dut_t;
	TH2D* h_cmn_dut_b;
	TH2D* h_cmn_fix_t;
	TH2D* h_cmn_fix_b;
	  
	TH1D* h_autocorrelation_dut_A;
	TH1D* h_autocorrelation_dut_B;
	TH1D* h_autocorrelation_fix_A;
	TH1D* h_autocorrelation_fix_B;
	
	TH1D* h_autocorrelation_dut_t;
	TH1D* h_autocorrelation_dut_b;
	TH1D* h_autocorrelation_fix_t;
	TH1D* h_autocorrelation_fix_b;
	
	// Occupancy Profiles
	TProfile2D* p_cmn_occ_dut_A;
	TProfile2D* p_cmn_occ_dut_B;
	TProfile2D* p_cmn_occ_fix_A;
	TProfile2D* p_cmn_occ_fix_B;
	
	TProfile2D* p_cmn_occ_dut_t;
	TProfile2D* p_cmn_occ_dut_b;
	TProfile2D* p_cmn_occ_fix_t;
	TProfile2D* p_cmn_occ_fix_b;
	
	// Correlation Plots
	TH2D* p_cmn_cor_dut_A;  
	TH2D* p_cmn_cor_dut_B;  
	TH2D* p_cmn_cor_fix_A;  
	TH2D* p_cmn_cor_fix_B;  
	
	TH2D* p_cmn_cor_dut_t;  
	TH2D* p_cmn_cor_dut_b;  
	TH2D* p_cmn_cor_fix_t;  
	TH2D* p_cmn_cor_fix_b;  
	
	int n_events;
	
private:
	virtual void beginJob() override;
	virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
	virtual void endJob() override;
	
	
	std::string bad_strip_file;
	std::map<int, std::set<int> > bad_strips;
	void read_bad_strips(std::string);
	bool strip_masked(unsigned int detid, int strip);
     
};


//
CMN_Analysis::CMN_Analysis(const edm::ParameterSet& iConfig)

{
	//now do what ever initialization is needed
	//now do what ever initialization is needed
	bad_strip_file = iConfig.getParameter<std::string>( "bad_strip_file" );
	read_bad_strips(bad_strip_file);
}


CMN_Analysis::~CMN_Analysis()
{
 
	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
	CMN_Analysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	
	int nhits_dut_t_A = 0;
	int nhits_dut_b_A = 0;
	int nhits_dut_t_B = 0;
	int nhits_dut_b_B = 0;
	
	int nhits_fix_t_A = 0;
	int nhits_fix_b_A = 0;
	int nhits_fix_t_B = 0;
	int nhits_fix_b_B = 0;
	
	int nhits_dut_A = 0;
	int nhits_dut_B = 0;
	int nhits_fix_A = 0;
	int nhits_fix_B = 0;
	
	// arrays to "un-zero-supress data for correlation plots!"
	int dut[508] = {0};
	int fix[508] = {0};
	
	//access Digis 
	iEvent.getByLabel("SiStripDigitestproducer", "ProcessedRaw", cbcDigis_);
	
	DetSetVector<PixelDigi>::const_iterator DSViter = cbcDigis_->begin();
	for (; DSViter!=cbcDigis_->end(); DSViter++) //module loop
	{
		int detid = DSViter->id;
		
		DetSet<PixelDigi>::const_iterator DSiter = DSViter->data.begin();
		for(; DSiter != DSViter->data.end(); DSiter++) // hit loop
		{
			int adc = DSiter->adc();
			
			if (adc > 250 && !strip_masked(detid,DSiter->row()))
			{
				//Fill strip number of every hit in Histo - > Beam Profile
				switch (detid)
				{
					case 51001: 
					{
						// dut_t[DSiter->row()] = 1;
						dut[2*DSiter->row()] = 1;
						if (n_events < 10000) h_cmn_dut_t->Fill(DSiter->row(),n_events);
						if (DSiter->row() < 127) nhits_dut_t_A++, nhits_dut_A++;
						else if (DSiter->row() >= 127) nhits_dut_t_B++, nhits_dut_B++;
						break; //CNM top
					}
					case 51002: 
					{
						// dut_b[DSiter->row()] = 1;
						dut[2*DSiter->row() + 1] = 1;
						if (n_events < 10000) h_cmn_dut_b->Fill(DSiter->row(),n_events);
						if (DSiter->row() < 127) nhits_dut_b_A++, nhits_dut_A++;
						else if (DSiter->row() >= 127) nhits_dut_b_B++, nhits_dut_B++;
						break; //CNM bottom
					}
					case 51011: 
					{
						// fix_t[DSiter->row()] = 1;
						fix[2*DSiter->row()] = 1;
						if (n_events < 10000) h_cmn_fix_t->Fill(DSiter->row(),n_events);
						if (DSiter->row() < 127) nhits_fix_t_A++, nhits_fix_A++;
						else if (DSiter->row() >= 127) nhits_fix_t_B++, nhits_fix_B++;
						break; //Infineon top
					}
					case 51012: 
					{
						// fix_b[DSiter->row()] = 1;
						fix[2*DSiter->row() + 1] = 1;
						if (n_events < 10000) h_cmn_fix_b->Fill(DSiter->row(),n_events);
						if (DSiter->row() < 127) nhits_fix_b_A++, nhits_fix_A++;
						else if (DSiter->row() >= 127) nhits_fix_b_B++, nhits_fix_B++;
						break;
					}
				}
			}
		} //End of hit loop
	} //End of module loop
	
	// fill occupancy plots
	for (int i = 0; i < 508; i++)
	{
		// initialize fill values for autocorrelogram
		double fill_dut_A = 0;
		double fill_dut_B = 0;
		
		double fill_fix_A = 0;
		double fill_fix_B = 0;
		
		double fill_dut_t = 0;
		double fill_dut_b = 0;
		
		double fill_fix_t = 0;
		double fill_fix_b = 0;
		
		for (int j = 0; j < 508; j++)
		{
			int fill_value_dut = 0;
			if (dut[i] && dut[j]) fill_value_dut = 1;
			
			int fill_value_fix = 0;
			if (fix[i] && fix[j]) fill_value_fix = 1;
			
			// per cbc
			if (i < 254 && j < 254) // CBC A
			{
				// occupancy profile
				p_cmn_occ_dut_A->Fill(i,j,fill_value_dut);
				p_cmn_occ_fix_A->Fill(i,j,fill_value_fix);
				
				// fill value for autocorrelogram
				fill_dut_A += (2*dut[j]-1)*(2*dut[(j+i)%254]-1);
				fill_fix_A += (2*fix[j]-1)*(2*fix[(j+i)%254]-1);
			}
			else if (i >= 254 && j >= 254) // CBC B
			{
				p_cmn_occ_dut_B->Fill(i-254,j-254,fill_value_dut);
				p_cmn_occ_fix_B->Fill(i-254,j-254,fill_value_fix);
				
				// fill value for autocorrelogram
				fill_dut_B += (2*dut[j]-1)*(2*dut[(j+i)%254]-1);
				fill_fix_B += (2*fix[j]-1)*(2*fix[(j+i)%254]-1);
			}
			
			// per sensor
			if (i%2 == 0 && j%2 == 0) // even numbers, top sensor
			{
				p_cmn_occ_dut_t->Fill(i/2,j/2,fill_value_dut);
				p_cmn_occ_fix_t->Fill(i/2,j/2,fill_value_fix);
				
				// fill value for autocorrelogram
				fill_dut_t += (2*dut[j]-1)*(2*dut[(j+i)%254]-1);
				fill_fix_t += (2*fix[j]-1)*(2*fix[(j+i)%254]-1);
			}
			else if (i%2 != 0 && j%2 != 0)  // odd numbers, bottom sensor
			{
				p_cmn_occ_dut_b->Fill(int(i/2),int(j/2),fill_value_dut);
				p_cmn_occ_fix_b->Fill(int(i/2),int(j/2),fill_value_fix);
				
				// fill value for autocorrelogram
				fill_dut_b += (2*dut[j]-1)*(2*dut[(j+i)%254]-1);
				fill_fix_b += (2*fix[j]-1)*(2*fix[(j+i)%254]-1);
			}
			
		}
		if (i < 254) // CBC A
		{
			h_autocorrelation_dut_A->SetBinContent(i,fill_dut_A);
			h_autocorrelation_fix_A->SetBinContent(i,fill_fix_A);
		}
		else if (i >= 254)
		{
			h_autocorrelation_dut_B->SetBinContent(i-254,fill_dut_B);
			h_autocorrelation_fix_B->SetBinContent(i-254,fill_fix_B);
		}
		if (i%2 == 0) // top sensor
		{
			h_autocorrelation_dut_t->SetBinContent(i/2,fill_dut_t);
			h_autocorrelation_fix_t->SetBinContent(i/2,fill_fix_t);
		} 
		else if (i%2 != 0) // top sensor
		{
			h_autocorrelation_dut_b->SetBinContent(i/2,fill_dut_b);
			h_autocorrelation_fix_b->SetBinContent(i/2,fill_fix_b);
		} 
	}
	
	//Number of hits per chip for CM Noise
	h_n_hits_dut_t_A->Fill(nhits_dut_t_A);
	h_n_hits_dut_b_A->Fill(nhits_dut_b_A);
	h_n_hits_dut_t_B->Fill(nhits_dut_t_B);
	h_n_hits_dut_b_B->Fill(nhits_dut_b_B);
	
	h_n_hits_fix_t_A->Fill(nhits_fix_t_A);
	h_n_hits_fix_b_A->Fill(nhits_fix_b_A);
	h_n_hits_fix_t_B->Fill(nhits_fix_t_B);
	h_n_hits_fix_b_B->Fill(nhits_fix_b_B);
	
	h_n_hits_dut_A->Fill(nhits_dut_A);
	h_n_hits_dut_B->Fill(nhits_dut_B);
	h_n_hits_fix_A->Fill(nhits_fix_A);
	h_n_hits_fix_B->Fill(nhits_fix_B);
	
	n_events++;
}


// ------------ method called once each job just before starting event loop  ------------
void CMN_Analysis::beginJob()
{
	n_events = 0;
	
	edm::Service<TFileService> fs;
    TFileDirectory dutDir = fs->mkdir( "DUT" );
    TFileDirectory fixDir = fs->mkdir( "FIX" );
	
	//Hit Distribution
	h_n_hits_dut_t_A = dutDir.make<TH1D>("h_n_hits_dut_t_A","Number of Hits DUT_T chip A", 128,-0.5,127.5);
	h_n_hits_dut_t_B = dutDir.make<TH1D>("h_n_hits_dut_t_B","Number of Hits DUT_T chip B", 128,-0.5,127.5);
	h_n_hits_dut_b_A = dutDir.make<TH1D>("h_n_hits_dut_b_A","Number of Hits DUT_B chip A", 128,-0.5,127.5);
	h_n_hits_dut_b_B = dutDir.make<TH1D>("h_n_hits_dut_b_B","Number of Hits DUT_B chip B", 128,-0.5,127.5);
	
	h_n_hits_fix_t_A = fixDir.make<TH1D>("h_n_hits_fix_t_A","Number of Hits FIX_T chip A", 128,-0.5,127.5);
	h_n_hits_fix_t_B = fixDir.make<TH1D>("h_n_hits_fix_t_B","Number of Hits FIX_T chip B", 128,-0.5,127.5);
	h_n_hits_fix_b_A = fixDir.make<TH1D>("h_n_hits_fix_b_A","Number of Hits FIX_B chip A", 128,-0.5,127.5);
	h_n_hits_fix_b_B = fixDir.make<TH1D>("h_n_hits_fix_b_B","Number of Hits FIX_B chip B", 128,-0.5,127.5);
	
	h_n_hits_dut_A = dutDir.make<TH1D>("h_n_hits_dut_A","Number of Hits DUT chip A", 255,-0.5,254.5);
	h_n_hits_dut_B = dutDir.make<TH1D>("h_n_hits_dut_B","Number of Hits DUT chip B", 255,-0.5,254.5);
	h_n_hits_fix_A = fixDir.make<TH1D>("h_n_hits_fix_A","Number of Hits FIX chip A", 255,-0.5,254.5);
	h_n_hits_fix_B = fixDir.make<TH1D>("h_n_hits_fix_B","Number of Hits FIX chip B", 255,-0.5,254.5);
	
	// 2D Histograms for CMN Analysis
	h_cmn_dut_t = dutDir.make<TH2D>("h_cmn_dut_t","CMN Raw Data Plot DUT top",254,-.5,254.5,10000,0,10000);
	h_cmn_dut_b = dutDir.make<TH2D>("h_cmn_dut_b","CMN Raw Data Plot DUT bottom",254,-.5,254.5,10000,0,10000);
	
	h_cmn_fix_t = fixDir.make<TH2D>("h_cmn_fix_t","CMN Raw Data Plot FIX top",254,-.5,254.5,10000,0,10000);
	h_cmn_fix_b = fixDir.make<TH2D>("h_cmn_fix_b","CMN Raw Data Plot FIX bottom",254,-.5,254.5,10000,0,10000);
	
	h_autocorrelation_dut_A = dutDir.make<TH1D>("h_autocorrelation_dut_A","Autocorrelation Plot DUT CBC A",255,-0.5,254.5);
	h_autocorrelation_dut_B = dutDir.make<TH1D>("h_autocorrelation_dut_B","Autocorrelation Plot DUT CBC B",255,-0.5,254.5);
	h_autocorrelation_fix_A = fixDir.make<TH1D>("h_autocorrelation_fix_A","Autocorrelation Plot FIX CBC A",255,-0.5,254.5);
	h_autocorrelation_fix_B = fixDir.make<TH1D>("h_autocorrelation_fix_B","Autocorrelation Plot FIX CBC B",255,-0.5,254.5);
	
	h_autocorrelation_dut_t = dutDir.make<TH1D>("h_autocorrelation_dut_t","Autocorrelation Plot DUT top",255,-0.5,254.5);
	h_autocorrelation_dut_b = dutDir.make<TH1D>("h_autocorrelation_dut_b","Autocorrelation Plot DUT bottom",255,-0.5,254.5);
	h_autocorrelation_fix_t = fixDir.make<TH1D>("h_autocorrelation_fix_t","Autocorrelation Plot FIX top",255,-0.5,254.5);
	h_autocorrelation_fix_b = fixDir.make<TH1D>("h_autocorrelation_fix_b","Autocorrelation Plot FIX bottom",255,-0.5,254.5);
	
	p_cmn_occ_dut_A = dutDir.make<TProfile2D>("p_cmn_occ_dut_A","CMN Occupancy Plot DUT CBC A",255,-.5,254.5,255,-.5,254.5);
	p_cmn_occ_dut_B = dutDir.make<TProfile2D>("p_cmn_occ_dut_B","CMN Occupancy Plot DUT CBC B",255,-.5,254.5,255,-.5,254.5);
	p_cmn_occ_fix_A = fixDir.make<TProfile2D>("p_cmn_occ_fix_A","CMN Occupancy Plot FIX CBC A",255,-.5,254.5,255,-.5,254.5);
	p_cmn_occ_fix_B = fixDir.make<TProfile2D>("p_cmn_occ_fix_B","CMN Occupancy Plot FIX CBC B",255,-.5,254.5,255,-.5,254.5);
	
	p_cmn_occ_dut_t = dutDir.make<TProfile2D>("p_cmn_occ_dut_t","CMN Occupancy Plot DUT top",255,-.5,254.5,255,-.5,254.5);
	p_cmn_occ_dut_b = dutDir.make<TProfile2D>("p_cmn_occ_dut_b","CMN Occupancy Plot DUT bottom",255,-.5,254.5,255,-.5,254.5);
	p_cmn_occ_fix_t = fixDir.make<TProfile2D>("p_cmn_occ_fix_t","CMN Occupancy Plot FIX top",255,-.5,254.5,255,-.5,254.5);
	p_cmn_occ_fix_b = fixDir.make<TProfile2D>("p_cmn_occ_fix_b","CMN Occupancy Plot FIX bottom",255,-.5,254.5,255,-.5,254.5);
	
	p_cmn_cor_dut_A = dutDir.make<TH2D>("p_cmn_cor_dut_A","CMN Correlation Plot DUT CBC A",255,-.5,254.5,255,-.5,254.5);
	p_cmn_cor_dut_B = dutDir.make<TH2D>("p_cmn_cor_dut_B","CMN Correlation Plot DUT CBC B",255,-.5,254.5,255,-.5,254.5);
	p_cmn_cor_fix_A = fixDir.make<TH2D>("p_cmn_cor_fix_A","CMN Correlation Plot FIX CBC A",255,-.5,254.5,255,-.5,254.5);
	p_cmn_cor_fix_B = fixDir.make<TH2D>("p_cmn_cor_fix_B","CMN Correlation Plot FIX CBC B",255,-.5,254.5,255,-.5,254.5);
	
	p_cmn_cor_dut_t = dutDir.make<TH2D>("p_cmn_cor_dut_t","CMN Correlation Plot DUT top",255,-.5,254.5,255,-.5,254.5);
	p_cmn_cor_dut_b = dutDir.make<TH2D>("p_cmn_cor_dut_b","CMN Correlation Plot DUT bottom",255,-.5,254.5,255,-.5,254.5);
	p_cmn_cor_fix_t = fixDir.make<TH2D>("p_cmn_cor_fix_t","CMN Correlation Plot FIX top",255,-.5,254.5,255,-.5,254.5);
	p_cmn_cor_fix_b = fixDir.make<TH2D>("p_cmn_cor_fix_b","CMN Correlation Plot FIX bottom",255,-.5,254.5,255,-.5,254.5);
}

// ------------ method called once each job just after ending the event loop  ------------
void 
	CMN_Analysis::endJob() 
{
	// fill another TH2D with frac(Oxy-OxOy)(sqrt(Ox-Ox^2)*sqrt(Oy-Oy^2)) 
	// where Oxy is TProfile2D->GetBinContent(x,y) and Ox is TProfile2D->GetBinContent(x,x)
	// GetBinContent is overloaded with []
	for (int i = 0; i < 255; i++)
	{
		for (int j = 0; j < 255; j++)
		{
			p_cmn_cor_dut_A->SetBinContent(i,j,(p_cmn_occ_dut_A->GetBinContent(i,j) - p_cmn_occ_dut_A->GetBinContent(i,i)*p_cmn_occ_dut_A->GetBinContent(j,j))/(sqrt(p_cmn_occ_dut_A->GetBinContent(i,i)-pow(p_cmn_occ_dut_A->GetBinContent(i,i),2))*sqrt(p_cmn_occ_dut_A->GetBinContent(j,j)-pow(p_cmn_occ_dut_A->GetBinContent(j,j),2))));
			
			p_cmn_cor_dut_B->SetBinContent(i,j,(p_cmn_occ_dut_B->GetBinContent(i,j) - p_cmn_occ_dut_B->GetBinContent(i,i)*p_cmn_occ_dut_B->GetBinContent(j,j)) / (sqrt(p_cmn_occ_dut_B->GetBinContent(i,i)-pow(p_cmn_occ_dut_B->GetBinContent(i,i),2))*sqrt(p_cmn_occ_dut_B->GetBinContent(j,j)-pow(p_cmn_occ_dut_B->GetBinContent(j,j),2))));
			
			p_cmn_cor_fix_A->SetBinContent(i,j,(p_cmn_occ_fix_A->GetBinContent(i,j) - p_cmn_occ_fix_A->GetBinContent(i,i)*p_cmn_occ_fix_A->GetBinContent(j,j)) / (sqrt(p_cmn_occ_fix_A->GetBinContent(i,i)-pow(p_cmn_occ_fix_A->GetBinContent(i,i),2))*sqrt(p_cmn_occ_fix_A->GetBinContent(j,j)-pow(p_cmn_occ_fix_A->GetBinContent(j,j),2))));
			
			p_cmn_cor_fix_B->SetBinContent(i,j,(p_cmn_occ_fix_B->GetBinContent(i,j) - p_cmn_occ_fix_B->GetBinContent(i,i)*p_cmn_occ_fix_B->GetBinContent(j,j)) / (sqrt(p_cmn_occ_fix_B->GetBinContent(i,i)-pow(p_cmn_occ_fix_B->GetBinContent(i,i),2))*sqrt(p_cmn_occ_fix_B->GetBinContent(j,j)-pow(p_cmn_occ_fix_B->GetBinContent(j,j),2))));
			
			
			p_cmn_cor_dut_t->SetBinContent(i,j,(p_cmn_occ_dut_t->GetBinContent(i,j) - p_cmn_occ_dut_t->GetBinContent(i,i)*p_cmn_occ_dut_t->GetBinContent(j,j)) / (sqrt(p_cmn_occ_dut_t->GetBinContent(i,i)-pow(p_cmn_occ_dut_t->GetBinContent(i,i),2))*sqrt(p_cmn_occ_dut_t->GetBinContent(j,j)-pow(p_cmn_occ_dut_t->GetBinContent(j,j),2))));
			
			p_cmn_cor_dut_b->SetBinContent(i,j,(p_cmn_occ_dut_b->GetBinContent(i,j) - p_cmn_occ_dut_b->GetBinContent(i,i)*p_cmn_occ_dut_b->GetBinContent(j,j)) / (sqrt(p_cmn_occ_dut_b->GetBinContent(i,i)-pow(p_cmn_occ_dut_b->GetBinContent(i,i),2))*sqrt(p_cmn_occ_dut_b->GetBinContent(j,j)-pow(p_cmn_occ_dut_b->GetBinContent(j,j),2))));
			
			p_cmn_cor_fix_t->SetBinContent(i,j,(p_cmn_occ_fix_t->GetBinContent(i,j) - p_cmn_occ_fix_t->GetBinContent(i,i)*p_cmn_occ_fix_t->GetBinContent(j,j)) / (sqrt(p_cmn_occ_fix_t->GetBinContent(i,i)-pow(p_cmn_occ_fix_t->GetBinContent(i,i),2))*sqrt(p_cmn_occ_fix_t->GetBinContent(j,j)-pow(p_cmn_occ_fix_t->GetBinContent(j,j),2))));
			
			p_cmn_cor_fix_b->SetBinContent(i,j,(p_cmn_occ_fix_b->GetBinContent(i,j) - p_cmn_occ_fix_b->GetBinContent(i,i)*p_cmn_occ_fix_b->GetBinContent(j,j)) / (sqrt(p_cmn_occ_fix_b->GetBinContent(i,i)-pow(p_cmn_occ_fix_b->GetBinContent(i,i),2))*sqrt(p_cmn_occ_fix_b->GetBinContent(j,j)-pow(p_cmn_occ_fix_b->GetBinContent(j,j),2))));
		}
	}
}

void
	CMN_Analysis::read_bad_strips(std::string bad_strip_file)
{
	//read the file with bad strips
	std::ifstream mask(bad_strip_file.c_str());
	if (!mask) std::cout << "No bad strip file " << bad_strip_file << " found, continuing wihtout!" << std::endl;
	while(!mask.eof())
	{
		std::string line;
		std::getline(mask, line);
		std::stringstream linestream(line);
		std::istream_iterator<int> begin(linestream), eos;
		int module;
		if (begin == eos) 
		{
			module = *begin;
			std::set<int> bad_strip_set;
			this->bad_strips[module] = bad_strip_set;
		}
		else
		{
			module = *begin;
			begin++;
			std::set<int> bad_strip_set (begin, eos);
			this->bad_strips[module] = bad_strip_set;
		}
	}
	
	for (std::map< int, std::set<int> >::iterator map_it = this->bad_strips.begin(); map_it != this->bad_strips.end(); map_it++)
	{
		std::cout << "Reading bad strips for Module " << map_it->first << " : ";
		for (std::set<int>::iterator it = map_it->second.begin(); it != map_it->second.end(); it++)
		{
			std::cout << *it << " ";
		}
		std::cout << std::endl;
	}
}

bool CMN_Analysis::strip_masked(unsigned int detid, int strip)
{
	if (this->bad_strips[detid].find(strip) != this->bad_strips[detid].end()) return true;
	else return false;
}

void CMN_Analysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(CMN_Analysis);
