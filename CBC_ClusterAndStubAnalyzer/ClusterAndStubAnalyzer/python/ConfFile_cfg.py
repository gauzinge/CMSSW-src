import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/g/gauzinge/tb_data/run487_clusters.root'
    )
)

#TfileService for Histograms
process.TFileService = cms.Service("TFileService",
    fileName = cms.string('/afs/cern.ch/user/g/gauzinge/mytesthisto.root')
)

process.demo = cms.EDAnalyzer('ClusterAndStubAnalyzer',
	sensors = cms.untracked.vuint32(50001, 50002, 50011, 50012),
	modules = cms.untracked.vuint32(50000, 50010)
)


process.p = cms.Path(process.demo)
