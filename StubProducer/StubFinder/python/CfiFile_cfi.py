import FWCore.ParameterSet.Config as cms

demo = cms.EDProducer('StubFinder',
         # Stub windowsize is the width of the search window to look for correlated hits. 
         stub_windowsize = cms.vint32(7),
         bad_strip_file = cms.string("/afs/cern.ch/user/g/gauzinge/tb_data/bad_strips.txt"),
)
