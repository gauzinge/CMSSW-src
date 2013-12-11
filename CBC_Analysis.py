import FWCore.ParameterSet.Config as cms

#set up a process , for e.g. Low Level Analysis in this case
process = cms.Process("LLAnalysis")


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('Demo')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

# Infile
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/g/gauzinge/tb_data/data_run313.root'
		# separate several with comma
    )
)

# Output module
# process.out = cms.OutputModule("PoolOutputModule",
#                 fileName = cms.untracked.string("test2.root")
#         )

#TfileService for Histograms
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("run313_analysis.root")
)

# process.demo = cms.EDAnalyzer('Analysis')
process.conditions = cms.EDAnalyzer('ConditionDecoder')
process.lowlevel = cms.EDAnalyzer('LL_Analysis')


# process.mypath = cms.Path(process.conditions)
process.mypath = cms.Path(process.conditions*process.lowlevel)