// -*- C++ -*-
//
// Package:    StubFinder
// Class:      StubFinder
// 
/**\class StubFinder StubFinder.cc StubProducer/StubFinder/plugins/StubFinder.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Georg Auzinger
//         Created:  Thu, 05 Dec 2013 15:34:15 GMT
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

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

#include <vector>
#include <iostream>
#include <set>
#include <map>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Phase2TrackerDigi/interface/Phase2TrackerDigi.h"
#include "DataFormats/Phase2TrackerCluster1D/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/Phase2TrackerStub/interface/Phase2TrackerStub.h"

//
// class declaration
//

// FIXME
// I am lacking all sorts of histograms

class StubFinder : public edm::EDProducer {
   public:
      explicit StubFinder(const edm::ParameterSet&);
      ~StubFinder();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	  
	  std::map<int, std::set<int> > bad_strips;
	  
   private:
      virtual void beginJob(const edm::EventSetup&) override;
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      
      // ----------member data ---------------------------
	  edm::InputTag src_;							//input file? whatever
      DetIdCollection detids_;
	  
	  //input parameter: windowsize
	  int stub_windowsize;
	  
	  // edm::DetSetVector<Phase2TrackerCluster>;
 // 	  edm::DetSetVector<Phase2TrackerStub>;
	  int n_events;				
};


StubFinder::StubFinder(const edm::ParameterSet& iConfig)
{
   //register your products
	src_  = iConfig.getParameter<edm::InputTag>( "src" ); 
	stub_windowsize = iConfig.getParameter( "stub_windowsize" );
	
    // produces< edm::DetSetVector<Phase2TrackerCluster1D> >("Clusters"); 
//     produces< edm::DetSetVector<Phase2TrackerStub> >("Stubs");
		
    produces< edm::DetSetVector<Phase2TrackerCluster1D> >("Clusters"); //.setBranchAlias("Cluster_branch"); 
    produces< edm::DetSetVector<Phase2TrackerStub> >("Stubs"); //.setBranchAlias("Stub_branch");
	
   //now do what ever other initialization is needed
	n_events = 0;
	
	int bad_dutt = {};
	int bad_dutb = {};
	int bad_fixt = {144,147,153};
	int bad_fixb = {134,143,144,145,146,147,148,150,152,153,154,155};
	
	bad_strips[51001] = std::set(bad_dutt, bad_dutt + sizeof(bad_dutt)/sizeof(int));
	bad_strips[51002] = std::set(bad_dutb, bad_dutb + sizeof(bad_dutb)/sizeof(int));
	bad_strips[51011] = std::set(bad_fixt, bad_fixt + sizeof(bad_fixt)/sizeof(int));
	bad_strips[51012] = std::set(bad_fixb, bad_fixb + sizeof(bad_fixb)/sizeof(int));
}


StubFinder::~StubFinder()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
StubFinder::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
   
    // use auto ptr to DetSetVector<Phase2TrackerCluster1D> to avoid memory leak
    auto_ptr< edm::DetSetVector<Phase2TrackerCluster1D> > Cluster_dsv( new edm::DetSetVector<Phase2TrackerCluster1D>);
    auto_ptr< edm::DetSetVector<Phase2TrackerStub> > Stub_dsv( new edm::DetSetVector<Phase2TrackerStub>);
    
    // handles for already existing input edm collections
    edm::Handle<edm::DetSetVector<PixelDigi> >  cbcDigis_;
    edm::Handle<edm::DetSet<SiStripCommissioningDigi> >  cbcCommissioningEvent_;
	
	DetSetVector<PixelDigi>::const_iterator DSViter = cbcDigis_->begin();
	for (; DSViter!=cbcDigis_->end(); DSViter++) //sensors
	{	
		//get id of Detector
	 	int detid = DSViter->id;
		
		int last_strip = -2;
		int clustersize = 0;
		
		//create a det set + det set immediately
		DetSet<Phase2TrackerCluster1D> cluster_ds(detid);
				
		DetSet<PixelDigi>::const_iterator DSiter = DSViter->data.begin();
		for(; DSiter != DSViter->data.end(); DSiter++) //loop over all hits on subdet
		{
			
			if (if (DSiter->adc() > 250 && bad_strips[detit].find(DSiter->row()) == bad_strips[detid].end())// hit is not in bad strips
			{
				// if adjacent to last, increment size, else write object and reset
				if (DSiter->row() == last_strip+1) clustersize += 1.;
				//1 strip cluster has clustersize 0
				else
				{
					//push cluster in DetSet cluster_ds
					cluster_ds.push_back(Phase2TrackerCluster1D(last_strip,0,clustersize));
					
					clustersize = 0;
					last_strip = DSiter->row();
				}
			}		
		} // loop over detsets
		
	// insert current detSet in DetSetVector
	Cluster_dsv->insert(cluster_ds);	
		
	} // loop over input DSV
	
	// sort cluster DSV 
	Cluster_dsv->sort();
	
	// write to EDM, this is just a try
	iEvent.put( Cluster_dsv, "Clusters" );
	
	// run stub finding here!
	// 2 DSV iterators, loop over ds in dsv as 1st iterator, 2nd iterator does a find in DSV and looks for detid that is exactly 1 larger from first det id. i stop this when i have finished DSV->size/2 times

	DetSetVector< edm::DetSetVector<Phase2TrackerCluster1D> >::const_iterator first_it = Cluster_dsv->begin();
	DetSetVector< edm::DetSetVector<Phase2TrackerCluster1D> >::const_iterator second_it = Cluster_dsv->begin();
	
	unsigned int sensors = Cluster_dsv->size();
	unsigned int module_counter = 0;
	
	for (; first_it != Cluster_dsv->end(); first_it++) //sensors
	{
		
		//FIXME 
		
		unsigned int first_id = first_it->id;
		unsigned int second_id = first_id + 1; //requires DSV->sort(), maybe there is a more elegant solution
		unsigned int module_id = first_id - 1; //this is not clean, same as above
		
		//FIXME 
		
		second_it = Cluster_dsv.find(second_id);
		
		if (second_it != Cluster_dsv->end()) // actually found matching sensor with det_id+1
		{
			//need DS of stubs here
			DetSet<Phase2TrackerStub> stub_ds(module_id);
			
			//this is the loop over the clusters on the 1st sensor
			DetSet< Phase2TrackerCluster1D >::const_iterator first_cluster_it = first_it->data.begin();
			for(; first_cluster_it != first_it->data.end(); first_cluster_it++) 
			{
				//this is the loop over the clusters on the 2nd sensor
				DetSet< Phase2TrackerCluster1D >::const_iterator second_cluster_it = second_it->data.begin();
				for(; second_cluster_it != second_it->data.end(); second_cluster_it++)
				{
					//compare clusters and create stub
					if ( fabs ( second_cluster_it.center() - first_cluster_it.center() ) <= stub_windowsize )
					{
						stub_ds.push_back(Phase2TrackerStub(second_cluster_it.center(),first_cluster_it.center() - second_cluster_it.center(), 0));
						//the convention is that channel comes from the bottom sensor, bx offset is 0 in case of a parallel test beam
					}
				}
			}
			
			//put stub ds in stub dsv
			Stub_dsv->insert(stub_ds);
		}
		else
		{
			//Throw an error
			std::cout << "No matching sensor found!" << std::endl;
		}
		
		module_counter++;
		if(module_counter == sensors/2) break; //as soon as I have matched sensor/2 modules quit, because I am done!
	}
	
   	n_events++;
	
	// and save the vectors
	// syntax: object, "produces(name)"
	// iEvent.put( Cluster_dsv, "Clusters" );
	iEvent.put( Stub_dsv, "Stubs" );
}

// ------------ method called once each job just before starting event loop  ------------
void 
StubFinder::beginJob()
{
	//eventually book some histos and write to file....
}

// ------------ method called once each job just after ending the event loop  ------------
void 
StubFinder::endJob() {
}

void
StubFinder::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  // edm::ParameterSetDescription desc;
  // desc.setUnknown();
  // descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(StubFinder);
