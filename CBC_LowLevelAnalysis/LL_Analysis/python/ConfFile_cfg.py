import FWCore.ParameterSet.Config as cms
# wad demo instead of LL_Analysis
process = cms.Process("LL_Analysis") 

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:myfile.root'
    )
)

process.demo = cms.EDAnalyzer('LL_Analysis',
		 bad_strip_file = cms.string("/afs/cern.ch/user/g/gauzinge/tb_data/bad_strips.txt")

)


process.p = cms.Path(process.demo)
