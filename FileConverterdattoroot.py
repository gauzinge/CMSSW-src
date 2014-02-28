import FWCore.ParameterSet.Config as cms 
import sys
import string

# Filenames for in and outfile
if len(sys.argv) != 3:
	print 'Wrong number of Command line arguments. Usage: cmsRun', sys.argv[1], 'datafile!\n\n' 

# runnumber = re.findall("USC.00000(\d+)",sys.argv[2])
infile = 'file:'+sys.argv[2]
outfile = infile[:-3]+'root'
outfile = outfile.replace("/dat/","/digis/")

print 'Reading file', infile
print 'Writing results to ', outfile 
# process declaration
process = cms.Process("OFFLINE") 


# message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi") 
process.MessageLogger.cerr.FwkReport.reportEvery = 10000 
process.MessageLogger.cerr.threshold = 'WARNING'
process.MessageLogger.categories.append('OFFLINE')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)

 
process.source = cms.Source("NewEventStreamFileReader",
    #fileNames = cms.untracked.vstring("file:/opt/cmssw/Data/closed/USC.00000089.0001.A.storageManager.00.0000.dat")
    fileNames = cms.untracked.vstring( infile)

)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


## EventContentAnalyzer
process.ana = cms.EDAnalyzer("EventContentAnalyzer")


## Output module
process.out = cms.OutputModule("PoolOutputModule",
#    outputCommands = cms.untracked.vstring(
#        'keep *',
#        'drop *_SiStripDigiCondDataproducer_ConditionData_HLT'
##        'keep *_siStripDigis_*_*'
#    ),
   # fileName = cms.untracked.string('data_run'+sys.argv[-1]+'.root')
    fileName = cms.untracked.string(outfile)
)

process.p1 = cms.Path(process.ana)
process.p2 = cms.EndPath(process.out)



