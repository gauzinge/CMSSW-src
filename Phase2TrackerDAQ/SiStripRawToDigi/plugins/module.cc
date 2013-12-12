#include "FWCore/Framework/interface/MakerMacros.h"


#include "Phase2TrackerDAQ/SiStripRawToDigi/plugins/SiStripDigi_test_producer.h"
#include "Phase2TrackerDAQ/SiStripRawToDigi/plugins/SiStripDigi_CondData_producer.h"

typedef sistrip::SiStripDigi_test_producer SiStripDigi_test_producer;
typedef sistrip::SiStripDigi_CondData_producer SiStripDigi_CondData_producer;

DEFINE_FWK_MODULE(SiStripDigi_test_producer);
DEFINE_FWK_MODULE(SiStripDigi_CondData_producer);
