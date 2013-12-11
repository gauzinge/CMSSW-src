import FWCore.ParameterSet.Config as cms
import sys
import re

# Filenames for in and outfile
if len(sys.argv) != 3:
	print 'Wrong number of Command line arguments. Usage: cmsRun', sys.argv[1], 'datafile!\n\n' 

runnumber = re.findall("data_run(\d+)",sys.argv[2])
datafile = 'file:'+sys.argv[2]
outfile = '/afs/cern.ch/user/g/gauzinge/tb_results/run' + runnumber[0] + '_analysis.root'

print '\nThe runnumber is ',runnumber[0]
print '\nThe File to be read is ',datafile 
print '\nThe histograms are written to ', outfile
print '\n\n'

#set up a process , for e.g. Low Level Analysis in this case
process = cms.Process("LLAnalysis")


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('LLAnalysis')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)


process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

# Infile
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(datafile
		# separate several with comma
    )
)

# Output module
# process.out = cms.OutputModule("PoolOutputModule",
#                 fileName = cms.untracked.string("test2.root")
#         )

#TfileService for Histograms
process.TFileService = cms.Service("TFileService",
    fileName = cms.string(outfile)
)

# process.demo = cms.EDAnalyzer('Analysis')
process.conditions = cms.EDAnalyzer('ConditionDecoder')
process.lowlevel = cms.EDAnalyzer('LL_Analysis')


process.mypath = cms.Path(process.conditions*process.lowlevel)