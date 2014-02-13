import FWCore.ParameterSet.Config as cms

process = cms.Process("CLUANDSTUBRECO")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/g/gauzinge/tb_data/USC.00000487.0001.A.storageManager.00.0000.root'
    )
)

process.StubFinder = cms.EDProducer('ClusterAndStubFinder',
         # Stub windowsize is the width of the search window to look for correlated hits. 
         stub_windowsize = cms.uint32(7)
)

#TfileService for Histograms
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("/afs/cern.ch/user/g/gauzinge/tb_results/myHistoFile.root")
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('file:/afs/cern.ch/user/g/gauzinge/tb_data/run487_clusters.root')
)

  
process.p = cms.Path(process.StubFinder)

process.e = cms.EndPath(process.out)
