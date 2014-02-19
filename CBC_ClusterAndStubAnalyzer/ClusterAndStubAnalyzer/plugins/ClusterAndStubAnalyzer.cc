// -*- C++ -*-
//
// Package:    ClusterAndStubAnalyzer
// Class:      ClusterAndStubAnalyzer
// 
/**\class ClusterAndStubAnalyzer ClusterAndStubAnalyzer.cc CBC_ClusterAndStubAnalyzer/ClusterAndStubAnalyzer/plugins/ClusterAndStubAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Georg Auzinger
//         Created:  Thu, 13 Feb 2014 14:47:00 GMT
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "Phase2TrackerDAQ/SiStripDigi/interface/SiStripCommissioningDigi.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// Custom Data formats
#include "DataFormats/Phase2TrackerDigi/interface/Phase2TrackerDigi.h"
#include "DataFormats/Phase2TrackerCluster1D/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/Phase2TrackerStub/interface/Phase2TrackerStub.h"

// Root includes
#include "TH1D.h"

// STL includes
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
// class declaration
//



class ClusterAndStubAnalyzer : public edm::EDAnalyzer {
   public:
      explicit ClusterAndStubAnalyzer(const edm::ParameterSet&);
      ~ClusterAndStubAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
	  
	  // Data handles
      edm::Handle<edm::DetSetVector<Phase2TrackerCluster1D> >  clusters_;
      edm::Handle<edm::DetSetVector<Phase2TrackerStub> >  stubs_;
	  
	  TH1D* h_n_clusters_DUT_T; TH1D* h_n_clusters_DUT_B; TH1D* h_n_clusters_FIX_T; TH1D* h_n_clusters_FIX_B;
	  TH1D* h_clu_size_DUT_T; TH1D* h_clu_size_DUT_B; TH1D* h_clu_size_FIX_T; TH1D* h_clu_size_FIX_B;
	  TH1D* h_clu_center_DUT_T; TH1D* h_clu_center_DUT_B; TH1D* h_clu_center_FIX_T; TH1D* h_clu_center_FIX_B;
	       
	  TH1D* h_n_stubs_DUT; TH1D* h_n_stubs_FIX;
	  TH1D* h_stub_bend_DUT; TH1D* h_stub_bend_FIX;
	  TH1D* h_stub_center_DUT; TH1D* h_stub_center_FIX;
	  
	  std::vector<unsigned int> sensors;
	  std::vector<unsigned int> modules;
	  
	  int n_events;				
	  
      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ClusterAndStubAnalyzer::ClusterAndStubAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
	sensors = iConfig.getUntrackedParameter<std::vector<unsigned int> >( "sensors" );
	modules = iConfig.getUntrackedParameter<std::vector<unsigned int >>( "modules" );
	
	n_events = 0;
}


ClusterAndStubAnalyzer::~ClusterAndStubAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
	ClusterAndStubAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;

	iEvent.getByLabel("stubfinder","Clusters",clusters_);
	DetSetVector<Phase2TrackerCluster1D>::const_iterator cluster_dsv = clusters_->begin();
	
	for (; cluster_dsv!=clusters_->end(); cluster_dsv++) //module loop
	{
		unsigned int detid = cluster_dsv->id;
		switch(detid)
		{
			case 51001: h_n_clusters_DUT_T->Fill(cluster_dsv->data.size()); break;
			case 51002: h_n_clusters_DUT_B->Fill(cluster_dsv->data.size()); break;
			case 51011: h_n_clusters_FIX_T->Fill(cluster_dsv->data.size()); break;
			case 51012: h_n_clusters_FIX_B->Fill(cluster_dsv->data.size()); break;
			
		}
		DetSet<Phase2TrackerCluster1D>::const_iterator cluster_ds = cluster_dsv->data.begin();
		
		for(; cluster_ds != cluster_dsv->data.end(); cluster_ds++) // hit loop
		{
			// std::cout << detid << cluster_ds->size() << " " << cluster_ds->center() << " " << cluster_dsv->data.size()<<std::endl;
			switch(detid)
			{                                                                                                             
				case 51001: h_clu_size_DUT_T->Fill(cluster_ds->size()+1); h_clu_center_DUT_T->Fill(cluster_ds->center()); break;
				case 51002: h_clu_size_DUT_B->Fill(cluster_ds->size()+1); h_clu_center_DUT_B->Fill(cluster_ds->center()); break;
				case 51011: h_clu_size_FIX_T->Fill(cluster_ds->size()+1); h_clu_center_FIX_T->Fill(cluster_ds->center()); break;
				case 51012: h_clu_size_FIX_B->Fill(cluster_ds->size()+1); h_clu_center_FIX_B->Fill(cluster_ds->center()); break;
			}
			// h_clu_size[detid]->Fill(cluster_ds->size()+1);
// 			h_clu_center[detid]->Fill(cluster_ds->center());
		}
	}
	iEvent.getByLabel("stubfinder","Stubs",stubs_);
   
	DetSetVector<Phase2TrackerStub>::const_iterator stub_dsv = stubs_->begin();

	for (; stub_dsv!=stubs_->end(); stub_dsv++) //module loop
	{
		unsigned int detid = stub_dsv->id;
		switch(detid)
		{
			case 51000: h_n_stubs_DUT->Fill(stub_dsv->data.size()); break;
			case 51010: h_n_stubs_FIX->Fill(stub_dsv->data.size()); break;
		}
		DetSet<Phase2TrackerStub>::const_iterator stub_ds = stub_dsv->data.begin();
	
		for(; stub_ds != stub_dsv->data.end(); stub_ds++) // hit loop
		{
			// std::cout << stub_ds->triggerBend() << " " << stub_ds->barycenter().second << std::endl;
			switch(detid)
			{
				case 51000: h_stub_bend_DUT->Fill(stub_ds->triggerBend()); h_stub_center_DUT->Fill(stub_ds->barycenter().second); break;
				case 51010: h_stub_bend_FIX->Fill(stub_ds->triggerBend()); h_stub_center_FIX->Fill(stub_ds->barycenter().second); break;
			}
		}
	}
	
	n_events++;
}


// ------------ method called once each job just before starting event loop  ------------
void 
ClusterAndStubAnalyzer::beginJob()
{
	// Convention
	// 50001 = dut_top
	// 50002 = dut_bottom
	// 50011 = fix_top
	// 50012 = fix_bottom
	n_events = 0;
	
	edm::Service<TFileService> fs;
	
	h_n_clusters_DUT_T = fs->make<TH1D>("n_clusters_dut_t","n_clusters_dut_t",10,0,10);
	h_clu_size_DUT_T = fs->make<TH1D>("clu_size_dut_t","clu_size_dut_t",10,0,10);
	h_clu_center_DUT_T = fs->make<TH1D>("clu_pos_dut_t","clu_pos_dut_t",256,0,256);
			
	h_n_clusters_DUT_B = fs->make<TH1D>("n_clusters_dut_b","n_clusters_dut_b",10,0,10);
	h_clu_size_DUT_B = fs->make<TH1D>("clu_size_dut_b","clu_size_dut_b",10,0,10);
	h_clu_center_DUT_B = fs->make<TH1D>("clu_pos_dut_b","clu_pos_dut_b",256,0,256);
			
	h_n_clusters_FIX_T = fs->make<TH1D>("n_clusters_fix_t","n_clusters_fix_t",10,0,10);
	h_clu_size_FIX_T = fs->make<TH1D>("clu_size_fix_t","clu_size_fix_t",10,0,10);
	h_clu_center_FIX_T = fs->make<TH1D>("clu_pos_fix_t","clu_pos_fix_t",256,0,256);
			
	h_n_clusters_FIX_B = fs->make<TH1D>("n_clusters_fix_b","n_clusters_fix_b",10,0,10);
	h_clu_size_FIX_B = fs->make<TH1D>("clu_size_fix_b","clu_size_fix_b",10,0,10);
	h_clu_center_FIX_B = fs->make<TH1D>("clu_pos_fix_b","clu_pos_fix_b",256,0,256);
		
	
	
	h_n_stubs_DUT = fs->make<TH1D>("n_stubs_dut","n_stubs_dut",10,0,10);
	h_stub_bend_DUT = fs->make<TH1D>("stub_bend_dut","stub_bend_dut",10,0,10);
	h_stub_center_DUT = fs->make<TH1D>("stub_pos_dut","stub_pos_dut",256,0,256);
		
	h_n_stubs_FIX = fs->make<TH1D>("n_stubs_fix","n_stubs_fix",10,0,10);
	h_stub_bend_FIX = fs->make<TH1D>("stub_bend_fix","stub_bend_fix",10,0,10);
	h_stub_center_FIX = fs->make<TH1D>("stub_pos_fix","stub_pos_fix",256,0,256);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
ClusterAndStubAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
ClusterAndStubAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
ClusterAndStubAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
ClusterAndStubAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
ClusterAndStubAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ClusterAndStubAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ClusterAndStubAnalyzer);
