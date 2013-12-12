import FWCore.ParameterSet.Config as cms

SiStripDigitestproducer = cms.EDProducer(
    'SiStripDigi_test_producer',
    ProductLabel = cms.InputTag("source")
)
