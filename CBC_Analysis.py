import FWCore.ParameterSet.Config as cms
import sys
import os
import re
import string

if "BADSTRIPS" in os.environ
	os.getenv(BADSTRIPS)
	badstripfile = "%sBADSTRIPS/bad_strips.txt"
else
	print 'Please set the $BADSTRIPS variable in the submitter script!'

# Filenames for in and outfile
if len(sys.argv) != 3:
	print 'Wrong number of Command line arguments. Usage: cmsRun', sys.argv[1], 'datafile!\n\n' 

# runnumber = re.findall("USC.00000(\d+)",sys.argv[2])
# runnumber = re.findall("run(\d+)",sys.argv[2])
datafile = 'file:'+sys.argv[2]
print 'datafilestring', datafile
if "USC" in datafile:
	runnumber = re.findall("USC.000(\d+)",sys.argv[2])
	histofile = datafile.replace("USC.000","run")
	histofile = histofile.replace(".0001.A.storageManager.00.0000","_results")
	histofile = histofile.replace("/digis/","/results/")
else:
	runnumber = re.findall("run(\d+)",sys.argv[2])
	histofile = datafile.replace("_clusters","_results")
	histofile = histofile.replace("/clusters/","/results/")


print '\nThe runnumber is ',runnumber[0]
print '\nThe File to be read is ',datafile 
print '\nThe histograms are written to ', histofile
print '\n'

#set up a process , for e.g. Low Level Analysis in this case
process = cms.Process("CBCAnalysis")


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('CBC_Analysis')
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

#TfileService for Histograms
process.TFileService = cms.Service("TFileService",
    fileName = cms.string(histofile)
)

# Conditions Data Decoder
process.conditions = cms.EDAnalyzer('ConditionDecoder')

# Low Level Analysis
process.lowlevel = cms.EDAnalyzer('LL_Analysis',
		 sensors = cms.untracked.vuint32(50001, 50002, 50011, 50012),
		 bad_strip_file = cms.string(badstripfile)
)

# CM Noise Analysis
process.cmn = cms.EDAnalyzer('CMN_Analysis',
		 bad_strip_file = cms.string(badstripfile)
)

# process.mypath = cms.Path(process.conditions*process.lowlevel)
if "USC" in datafile:
	process.mypath = cms.Path(process.conditions*process.lowlevel*process.cmn)
else:
	# bad Strips are already masked in the Cluster&Stub Producer
	process.stubs = cms.EDAnalyzer('ClusterAndStubAnalyzer',
		sensors = cms.untracked.vuint32(50001, 50002, 50011, 50012),
		modules = cms.untracked.vuint32(50000, 50010)
	)
	process.mypath = cms.Path(process.conditions*process.lowlevel*process.cmn*process.stubs)