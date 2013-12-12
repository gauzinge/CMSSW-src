import FWCore.ParameterSet.Config as cms
import sys

process = cms.Process("RawToDigi")

process.load("FWCore.MessageService.MessageLogger_cfi")

# process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

# Input source
process.source = cms.Source("NewEventStreamFileReader",
    fileNames = cms.untracked.vstring( 'file:'+sys.argv[-1])
)

process.load('Phase2TrackerDAQ.SiStripRawToDigi.SiStripDigi_test_producer_cfi')
process.load('Phase2TrackerDAQ.SiStripRawToDigi.SiStripDigi_CondData_producer_cfi')

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('myOutputFile.root')
)

# process.output = cms.OutputModule(
#     "PoolOutputModule",
#     fileName = cms.untracked.string('output.root'),
#     outputCommands = cms.untracked.vstring(
#       'drop *',
#       'keep *_*_*_DigiToRawToDigi'
#       )
#     )
  
process.p = cms.Path(process.SiStripDigitestproducer*process.SiStripDigiCondDataproducer)

process.e = cms.EndPath(process.out)
