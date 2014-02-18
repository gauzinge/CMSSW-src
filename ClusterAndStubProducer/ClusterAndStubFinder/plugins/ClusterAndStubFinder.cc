// -*- C++ -*-
//
// Package:    ClusterAndStubFinder
// Class:      ClusterAndStubFinder
// 
/**\class ClusterAndStubFinder ClusterAndStubFinder.cc StubProducer/ClusterAndStubFinder/plugins/ClusterAndStubFinder.cc

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
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

// Custom Data formats
#include "DataFormats/Phase2TrackerDigi/interface/Phase2TrackerDigi.h"
#include "DataFormats/Phase2TrackerCluster1D/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/Phase2TrackerStub/interface/Phase2TrackerStub.h"

// STL includes
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
//
// class declaration
//



class ClusterAndStubFinder : public edm::EDProducer {
   public:
      explicit ClusterAndStubFinder(const edm::ParameterSet&);
      ~ClusterAndStubFinder();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	  	  
   private:
      virtual void beginJob();
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
	  
	  void read_bad_strips(std::string);
	  bool strip_masked(unsigned int detid, int strip);
      int getDigit(int number, int pos);
	  
	  //input parameter: windowsize
	  unsigned int stub_windowsize;
	  //input parameter: bad strip file
	  std::string bad_strip_file;
	  
	  std::map<int, std::set<int> > bad_strips;
	  int n_events;				
};


ClusterAndStubFinder::ClusterAndStubFinder(const edm::ParameterSet& iConfig)
{
	//read input parameters!
	stub_windowsize = iConfig.getParameter<unsigned int>( "stub_windowsize" );
	bad_strip_file = iConfig.getParameter<std::string>( "bad_strip_file" );
			
    produces< edm::DetSetVector<Phase2TrackerCluster1D> >("Clusters"); //.setBranchAlias("Cluster_branch"); 
    produces< edm::DetSetVector<Phase2TrackerStub> >("Stubs"); //.setBranchAlias("Stub_branch");
	
   //now do what ever other initialization is needed
	n_events = 0;
	
// 	int bad_dutb[] = {};
// 	int bad_fixt[] = {144,147,153};
// 	int bad_fixb[] = {134,143,144,145,146,147,148,150,152,153,154,155};
	
	//read the file with bad strips
	read_bad_strips(bad_strip_file);
}


ClusterAndStubFinder::~ClusterAndStubFinder()
{
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
ClusterAndStubFinder::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace std;
   
	// use auto ptr to DetSetVector<Phase2TrackerCluster1D> to avoid memory leak
	auto_ptr< edm::DetSetVector<Phase2TrackerCluster1D> > Cluster_dsv( new edm::DetSetVector<Phase2TrackerCluster1D>);
	auto_ptr< edm::DetSetVector<Phase2TrackerStub> > Stub_dsv( new edm::DetSetVector<Phase2TrackerStub>);
	
	// handles for already existing input edm collections
	edm::Handle<edm::DetSetVector<PixelDigi> >  cbcDigis_;
	
	iEvent.getByLabel("SiStripDigitestproducer", "ProcessedRaw", cbcDigis_);
		
	DetSetVector<PixelDigi>::const_iterator DSViter = cbcDigis_->begin();

	for (; DSViter!=cbcDigis_->end(); DSViter++) //sensors
	{			
		//get id of Detector
		unsigned int detid = DSViter->id;
		
		int first_strip = 0;
		int clustersize = 0;
		
		//create a det set + det set immediately
		DetSet<Phase2TrackerCluster1D> cluster_ds(detid);
				
		DetSet<PixelDigi>::const_iterator DSiter = DSViter->data.begin();
		for(; DSiter != DSViter->data.end(); DSiter++) //loop over all hits on subdet
		{
			if (DSiter->adc() > 250 && !strip_masked(detid,DSiter->row()))// sanity check && not masked strip
			{
				// if 1st iteration, set first strip
				if (DSiter == DSViter->data.begin()) first_strip = DSiter->row();
				else
				{
					// if adjacent to last, increment size, else write object and reset
					if (DSiter->row() == first_strip + clustersize + 1) 
					{
						clustersize += 1.;
					}
					//1 strip cluster has clustersize 0
					else
					{
						//push cluster in DetSet cluster_ds
						Phase2TrackerCluster1D mycluster(first_strip,0,clustersize);
						cluster_ds.push_back(mycluster);
					
						//reset
						clustersize = 0;
						first_strip = DSiter->row();
					}
				}
			}		
		} // loop over detsets
		
		// anyway, if data is only 1 large or in any other case I need to write the last cluster
		if (first_strip && DSViter->data.size())
		{
			//push cluster in DetSet cluster_ds
			Phase2TrackerCluster1D mycluster(first_strip,0,clustersize);
			cluster_ds.push_back(mycluster);
		}
		
		// insert current detSet in DetSetVector
		Cluster_dsv->insert(cluster_ds);	
		
	} // loop over input DSV

	// run stub finding now

	DetSetVector<Phase2TrackerCluster1D>::const_iterator first_it = Cluster_dsv->begin();
	DetSetVector<Phase2TrackerCluster1D>::const_iterator second_it = Cluster_dsv->begin();
		
	for (; first_it != Cluster_dsv->end(); first_it++) //sensors
	{
		unsigned int first_id = first_it->id;
		
		//FIXME are we sure that DETID is not binary? Bitmask anyone?
		if (getDigit(first_id,0) == 1)
		{
			unsigned int second_id = first_id + 1;
			unsigned int module_id = first_id - 1;
			second_it = Cluster_dsv->find(second_id);
		
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
						int top_center = floor(first_cluster_it->center());
						int bot_center = floor(second_cluster_it->center());
						unsigned int bend = static_cast<unsigned int> (fabs ( bot_center - top_center ));
						
						//compare clusters and create stub
						if ( bend <= stub_windowsize )
						{
							Phase2TrackerStub mystub(static_cast<unsigned int> (bot_center) * 2, 1, bend, 0);
							
							stub_ds.push_back(mystub);
							//the convention is that channel comes from the bottom sensor, edge is 1 in BT, bx offset is 0 in case of a parallel test beam
							
						}
					}
				}
			
				//put stub ds in stub dsv
				Stub_dsv->insert(stub_ds);
				
			}
		}
	}
	
	// and save the vectors
	// syntax: object, "produces(name)"
	iEvent.put( Cluster_dsv, "Clusters" );
	iEvent.put( Stub_dsv, "Stubs" );
	n_events++;
}

// ------------ method called once each job just before starting event loop  ------------
void 
ClusterAndStubFinder::beginJob()
{
	// Convention
	// 50001 = dut_top
	// 50002 = dut_bottom
	// 50011 = fix_top
	// 50012 = fix_bottom
	
	// edm::Service<TFileService> fs;
// 	
// 	for (std::vector<unsigned int>::iterator sen_it = sensors.begin(); sen_it != sensors.end(); sen_it++)
// 	{
// 		std::stringstream sensor_id;
// 		sensor_id << *sen_it;
// 		
// 		h_n_clusters[*sen_it] = fs->make<TH1D>("n_clusters_"+*sensor_id.str().c_str(),"n_clusters_"+*sensor_id.str().c_str(),256,0,256);
// 		h_clu_size[*sen_it] = fs->make<TH1D>("clu_size_"+*sensor_id.str().c_str(),"clu_size_"+*sensor_id.str().c_str(),10,0,10);
// 		h_clu_center_pos[*sen_it] = fs->make<TH1D>("clu_pos_"+*sensor_id.str().c_str(),"clu_pos_"+*sensor_id.str().c_str(),256,0,256);
// 	}
// 	
// 	for (std::vector<unsigned int>::iterator mod_it = modules.begin(); mod_it != modules.end(); mod_it++)
// 	{
// 		std::stringstream module_id;
// 		module_id << *mod_it;
// 		
// 		h_n_stubs[*mod_it] = fs->make<TH1D>("n_stubs_"+*module_id.str().c_str(),"n_clusters_"+*module_id.str().c_str(),256,0,256);
// 		h_stub_bend[*mod_it] = fs->make<TH1D>("stub_bend_"+*module_id.str().c_str(),"stub_bend_"+*module_id.str().c_str(),10,0,10);
// 		h_stub_center_pos[*mod_it] = fs->make<TH1D>("stub_pos_"+*module_id.str().c_str(),"stub_pos_"+*module_id.str().c_str(),256,0,256);
// 	}
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ClusterAndStubFinder::endJob() {
}

void
ClusterAndStubFinder::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  // edm::ParameterSetDescription desc;
  // desc.setUnknown();
  // descriptions.addDefault(desc);
}

void
ClusterAndStubFinder::read_bad_strips(std::string bad_strip_file)
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

bool ClusterAndStubFinder::strip_masked(unsigned int detid, int strip)
{
	if (this->bad_strips[detid].find(strip) != this->bad_strips[detid].end()) return true;
	else return false;
}

int ClusterAndStubFinder::getDigit(int number, int pos)
{
	return (pos == 0) ? number % 10 : getDigit (number/10, --pos);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ClusterAndStubFinder);
