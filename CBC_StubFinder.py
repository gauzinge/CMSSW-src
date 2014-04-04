import FWCore.ParameterSet.Config as cms
import sys
import os
import re

os.getenv('BADSTRIPS',"/afs/cern.ch/user/g/gauzinge/tb_data")
badstripfile = "%sBADSTRIPS/bad_strips.txt"
print 'Bad Strip File:', badstripfile 

# Filenames for in and outfile
if len(sys.argv) != 3:
	print 'Wrong number of Command line arguments. Usage: cmsRun', sys.argv[1], 'datafile!\n\n' 

runnumber = re.findall("USC.000(\d+)",sys.argv[2])
datafile = 'file:'+sys.argv[2]
# outfile = '/afs/cern.ch/user/g/gauzinge/tb_data/clusters/run' + runnumber[0] + '_clusters.root'
outfile = datafile.replace("USC.000","run")
outfile = outfile.replace(".0001.A.storageManager.00.0000","_clusters")
outfile = outfile.replace("/digis/","/clusters/")

print '\nThe runnumber is ',runnumber[0]
print '\nThe File to be read is ',datafile 
print '\nThe clusters and stubs are written to ', outfile
print '\n'

#set up a process , for e.g. Low Level Analysis in this case
process = cms.Process("ClusterAndStubFinding")


process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('ClusterAndStubFinding')
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


# Stub Producer
process.stubfinder = cms.EDProducer('ClusterAndStubFinder',
         # Stub windowsize is the width of the search window to look for correlated hits. 
         stub_windowsize = cms.uint32(7),
		 bad_strip_file = cms.string(badstripfile)
)

# Output module for Stub Finder output
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(outfile)
)

# process.mypath = cms.Path(process.conditions*process.lowlevel)
process.mypath = cms.Path(process.stubfinder)
process.e = cms.EndPath(process.out)