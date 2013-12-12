import FWCore.ParameterSet.Config as cms

SiStripDigiCondDataproducer = cms.EDProducer(
    'SiStripDigi_CondData_producer',
    ProductLabel = cms.InputTag("source")
)
