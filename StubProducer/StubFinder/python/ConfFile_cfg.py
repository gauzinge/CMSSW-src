import FWCore.ParameterSet.Config as cms

process = cms.Process("OWNPARTICLES")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:myfile.root'
    )
)

process.StubFinder = cms.EDProducer('StubFinder',
         # Stub windowsize is the width of the search window to look for correlated hits. 
         stub_windowsize = cms.vint32(7),
		 # self-explanatory
         bad_strip_file = cms.string("/afs/cern.ch/user/g/gauzinge/tb_data/bad_strips.txt"),
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('myOutputFile.root')
)

  
process.p = cms.Path(process.StubFinder)

process.e = cms.EndPath(process.out)
