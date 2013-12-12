import FWCore.ParameterSet.Config as cms

FED_test_Analyzer = cms.EDAnalyzer("SiStripFED_test_Analyzer",
    InputLabel = cms.InputTag("source")
)


