#ifndef EventFilter_SiStripRawToDigi_SiStripFEDBuffer_CBC_H // {
#define EventFilter_SiStripRawToDigi_SiStripFEDBuffer_CBC_H

#include "boost/cstdint.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ostream>
#include <iostream>
#include <cstring>
#include "FWCore/Utilities/interface/Exception.h"
#include "Phase2TrackerDAQ/SiStripRawToDigi/interface/SiStripFEDDAQHeader.h"
#include "Phase2TrackerDAQ/SiStripRawToDigi/interface/SiStripFEDDAQTrailer.h"

namespace sistrip {

  // classes defined below on this file:
  // Registry
  // TrackerHeader_CBC,
  // FEDChannel_CBC,
  // FEDRawChannelUnpacker_CBC,
  // FEDZSChannelUnpacker_CBC,
  // FEDBuffer_CBC;

  class Registry {
  public:
    /// constructor
    Registry(uint32_t aDetid, uint16_t firstStrip, size_t indexInVector, uint16_t numberOfDigis) :
      detid(aDetid), first(firstStrip), index(indexInVector), length(numberOfDigis) {}
    /// < operator to sort registries
    bool operator<(const Registry &other) const {return (detid != other.detid ? detid < other.detid : first < other.first);}
    /// public data members
    uint32_t detid;
    uint16_t first;
    size_t index;
    uint16_t length;
  };

////////////////////////////////////////////////////////////////////////////////
//                         TrackerHeader_CBC 
////////////////////////////////////////////////////////////////////////////////

  // tracker headers for new CBC system
  class TrackerHeader_CBC
  {
    public:
      TrackerHeader_CBC();

      explicit TrackerHeader_CBC(const uint8_t* headerPointer);

      // getters:
      inline uint8_t getDataFormatVersion() { return dataFormatVersion_; }
      inline READ_MODE getDebugMode()   { return debugMode_; }

      inline uint8_t getEventType() { return eventType_; }
      inline FEDReadoutMode getReadoutMode() const { return readoutMode_; }
      inline uint8_t getConditionData() const { return conditionData_; }
      inline uint8_t getDataType() const { return dataType_; }

      inline uint64_t getGlibStatusCode() const { return glibStatusCode_; }
      inline uint16_t getNumberOfCBC() const { return numberOfCBC_; }

      // get pointer to Payload data after tracker head
      const uint8_t* getPointerToData() const { return pointerToData_;}
      // get Front-End Status (16 bits) ==> 16 bool
      std::vector<bool> frontendStatus() const;

      inline uint8_t getTrackerHeaderSize() { return trackerHeaderSize_; }

      // CBC status bits, according to debug mode 
      // (empty, 1bit per CBC, 8bits per CBC)
      std::vector<uint8_t> CBCStatus() const;

    private:
      // readers: read info from Tracker Header and store in local variables

      // version number (4 bits)
      uint8_t dataFormatVersion() const;
      // debug level (2 bits) :
      // 01 = full debug, 10 = CBC error mode, 00 = summary mode
      READ_MODE debugMode() const;
      // event type (4 bits):
      // RAW/ZS, condition data, data type (real or simulated)
      uint8_t eventType() const;
      // get readout mode (first bit of the above)
      FEDReadoutMode readoutMode() const;
      uint8_t conditionData() const;
      uint8_t dataType() const;
      // glib status registers code (38 bits)
      uint64_t glibStatusCode() const;
      // number of CBC chips (8 bits)
      uint16_t numberOfCBC() const;
      // get tracker size (see function) and pointer to end of header
      const uint8_t* pointerToData() ;
 
    private:
      void init();
      const uint8_t* trackerHeader_; // pointer to the begining of Tracker Header
      const uint8_t* pointerToData_; // pointer next to end of Tracker Header
      uint8_t trackerHeaderSize_;    // Tracker Header in bytes
      uint64_t header_first_word_;
      uint64_t header_second_word_;
      uint8_t  dataFormatVersion_; // shoud be 1
      READ_MODE  debugMode_;       // debug, error, sumary ...
      uint8_t  eventType_;         // contains readoutMode_, conditionData_ and dataType_
      FEDReadoutMode readoutMode_; // proc raw or zero suppress
      uint8_t conditionData_;      // condition data present or not
      uint8_t dataType_;           // data fake or real
      uint64_t glibStatusCode_;    // glib status registers
      uint16_t numberOfCBC_;       // Total number of connected CBC

  }; // end of TrackerHeader_CBC class

  inline TrackerHeader_CBC::TrackerHeader_CBC() { }

////////////////////////////////////////////////////////////////////////////////
//                         FEDChannel_CBC
////////////////////////////////////////////////////////////////////////////////
  // holds information about position of a channel in the buffer
  // for use by unpacker
  class FEDChannel_CBC
  {
    public:
      FEDChannel_CBC(const uint8_t*const data, const size_t offset,
                 const uint16_t length);

      //gets length from first 2 bytes (assuming normal FED channel)
      FEDChannel_CBC(const uint8_t*const data, const size_t offset);
      uint16_t length() const;
      const uint8_t* data() const;
      size_t offset() const;
      uint16_t cmMedian(const uint8_t apvIndex) const;
    private:
      friend class FEDBuffer_CBC;
      //third byte of channel data for normal FED channels
      uint8_t packetCode() const;
      const uint8_t* data_;
      size_t offset_;
      uint16_t length_;
  }; // end FEDChannel_CBC class

  // FEDChannel_CBC methods definitions {

/*  XXX: remove deprecated. length_ is not read any more. 
  inline FEDChannel_CBC::FEDChannel_CBC(const uint8_t*const data, const size_t offset)
    : data_(data), offset_(offset)
  { 
    length_ = ( data_[(offset_)^7] + (data_[(offset_+1)^7] << 8) ); 
  }
*/

  inline FEDChannel_CBC::FEDChannel_CBC(const uint8_t*const data, const size_t offset,
                                const uint16_t length)
    : data_(data), offset_(offset), length_(length)
  { }

  inline uint16_t FEDChannel_CBC::length() const
  { return length_; }

/* XXX: remove for same reasons as above. length is not part of the channel
  inline uint8_t FEDChannel_CBC::packetCode() const
  { return data_[(offset_+2)^7]; }
*/

  inline const uint8_t* FEDChannel_CBC::data() const
  { return data_; }

  inline size_t FEDChannel_CBC::offset() const
  { return offset_; }

  // end of FEDChannel_CBC methods definitions }

////////////////////////////////////////////////////////////////////////////////
//                         FEDRawChannelUnpacker_CBC
////////////////////////////////////////////////////////////////////////////////

  // unpacker for RAW CBC data
  // each bit of the channel is related to one strip
  class FEDRawChannelUnpacker_CBC
  {
  public:
    FEDRawChannelUnpacker_CBC(const FEDChannel_CBC& channel);
    uint8_t stripIndex();
    bool stripOn();
    bool hasData();
    FEDRawChannelUnpacker_CBC& operator ++ ();
    FEDRawChannelUnpacker_CBC& operator ++ (int);
  private:
    const uint8_t* data_;
    uint8_t currentOffset_;
    uint8_t currentStrip_;
    uint16_t valuesLeft_;
    uint8_t currentWord_;
    uint8_t bitInWord_;
  }; // end of FEDRawChannelUnpacker_CBC

  inline FEDRawChannelUnpacker_CBC::FEDRawChannelUnpacker_CBC(const FEDChannel_CBC& channel)
    : data_(channel.data()),
      currentOffset_(channel.offset()),
      currentStrip_(0),
      valuesLeft_((channel.length())*8 - STRIPS_PADDING),
      currentWord_(channel.data()[currentOffset_^7]),
      bitInWord_(0)
  {
  }

  inline bool FEDRawChannelUnpacker_CBC::stripOn()
  {
    return bool((currentWord_>>bitInWord_)&0x1);
  }

  inline uint8_t FEDRawChannelUnpacker_CBC::stripIndex()
  {
    return currentStrip_;
  }

  inline bool FEDRawChannelUnpacker_CBC::hasData()
  {
    return valuesLeft_;
  }

  inline FEDRawChannelUnpacker_CBC& FEDRawChannelUnpacker_CBC::operator ++ ()
  {
    bitInWord_++;
    currentStrip_++;
    if (bitInWord_ > 7) {
      bitInWord_ = 0;
      currentOffset_++;
      currentWord_ = data_[currentOffset_^7];
    }
    valuesLeft_--;
    return (*this);
  }
  
  inline FEDRawChannelUnpacker_CBC& FEDRawChannelUnpacker_CBC::operator ++ (int)
  {
    ++(*this); return *this;
  }

////////////////////////////////////////////////////////////////////////////////
//                         FEDZSChannelUnpacker_CBC
////////////////////////////////////////////////////////////////////////////////

 class FEDZSChannelUnpacker_CBC
  {
  public:
    FEDZSChannelUnpacker_CBC(const FEDChannel_CBC& channel);
    uint8_t clusterIndex();
    uint8_t clusterLength();
    bool hasData();
    FEDZSChannelUnpacker_CBC& operator ++ ();
    FEDZSChannelUnpacker_CBC& operator ++ (int);
  private:
    const uint8_t* data_;
    uint8_t currentOffset_;
    uint16_t valuesLeft_;
  };

  // unpacker for ZS CBC data
  inline FEDZSChannelUnpacker_CBC::FEDZSChannelUnpacker_CBC(const FEDChannel_CBC& channel)
    : data_(channel.data()),
      currentOffset_(channel.offset()),
      valuesLeft_(channel.length()/2)
  {
  }

  inline uint8_t FEDZSChannelUnpacker_CBC::clusterIndex()
  {
    return data_[currentOffset_^7];
  }

  inline uint8_t FEDZSChannelUnpacker_CBC::clusterLength()
  {
    return data_[(currentOffset_+1)^7];  
  }

  inline bool FEDZSChannelUnpacker_CBC::hasData()
  {
    return valuesLeft_;
  }

  inline FEDZSChannelUnpacker_CBC& FEDZSChannelUnpacker_CBC::operator ++ ()
  {
    currentOffset_ = currentOffset_+2;
    valuesLeft_--;
    return (*this);
  }
  
  inline FEDZSChannelUnpacker_CBC& FEDZSChannelUnpacker_CBC::operator ++ (int)
  {
    ++(*this); return *this;
  }

////////////////////////////////////////////////////////////////////////////////
//                         FEDBuffer_CBC                                      //
////////////////////////////////////////////////////////////////////////////////

  class FEDBuffer_CBC
  { 
    public:
      // gets data of one tracker FED to check, analyze and sort it
      FEDBuffer_CBC(const uint8_t* fedBuffer, const size_t fedBufferSize);
      ~FEDBuffer_CBC();

      //dump buffer to stream
      void dump(std::ostream& os) const;

      //methods to get parts of the buffer
      FEDDAQHeader daqHeader() const;
      FEDDAQTrailer daqTrailer() const;
      size_t bufferSize() const;
      TrackerHeader_CBC trackerHeader() const;
      const FEDChannel_CBC& channel(const uint8_t internalFEDChannelNum) const;
      std::map<uint32_t,uint32_t> conditionData();

      //methods to get info from DAQ header from FEDDAQHeader class
      FEDDAQEventType daqEventType() const;
      uint32_t daqLvl1ID() const;
      uint16_t daqBXID() const;
      uint16_t daqSourceID() const;

      //methods to get info from DAQ trailer from FEDDAQTrailer class
      uint32_t daqEventLengthIn64bitWords() const;
      uint32_t daqEventLengthInBytes() const;
      uint16_t daqCRC() const;
      FEDTTSBits daqTTSState() const;

      //methods to get info from the tracker header using TrackerHeader_CBC class
      FEDReadoutMode readoutMode() const;
      inline const uint8_t* getPointerToPayload()  const { return trackerHeader_.getPointerToData(); }
      inline const uint8_t* getPointerToCondData() const { return condDataPointer_; }
      inline const uint8_t* getPointerToTriggerData() const { return triggerPointer_; }

    private:
      const uint8_t* buffer_;
      const size_t bufferSize_;
      std::vector<FEDChannel_CBC> channels_;
      FEDDAQHeader daqHeader_;
      FEDDAQTrailer daqTrailer_;
      TrackerHeader_CBC trackerHeader_;
      const uint8_t* payloadPointer_;
      const uint8_t* condDataPointer_;
      const uint8_t* triggerPointer_;
      void findChannels();

    //////////////// Deprecated or dummy implemented methods ///////////////////
    public:
      // check methods
      inline bool doChecks() const { return true; }  // FEDBuffer
      inline bool checkNoFEOverflows() const { return true; } // FEDBufferBase
      inline bool doCorruptBufferChecks() const { return true; } // FEDBuffer

  }; // end of FEDBuffer class

  //// FEDBuffer methods definitions {

  //dump buffer to stream
  inline void FEDBuffer_CBC::dump(std::ostream& os) const
  {
    printHex(buffer_,bufferSize_,os);
  }


  // methods to get parts of the buffer
  inline FEDDAQHeader FEDBuffer_CBC::daqHeader() const
  { return daqHeader_; }

  inline FEDDAQTrailer FEDBuffer_CBC::daqTrailer() const
  { return daqTrailer_; }

  inline size_t FEDBuffer_CBC::bufferSize() const
  { return bufferSize_; }

  inline TrackerHeader_CBC FEDBuffer_CBC::trackerHeader() const
  { return trackerHeader_; }

  inline const FEDChannel_CBC& FEDBuffer_CBC::channel(const uint8_t internalFEDChannelNum) const
  { return channels_[internalFEDChannelNum]; }

  // methods to get info from DAQ header uses FEDDAQHeader class
  inline FEDDAQEventType FEDBuffer_CBC::daqEventType() const
  { return daqHeader_.eventType(); }

  inline uint32_t FEDBuffer_CBC::daqLvl1ID() const
  { return daqHeader_.l1ID(); }

  inline uint16_t FEDBuffer_CBC::daqBXID() const
  { return daqHeader_.bxID(); }

  inline uint16_t FEDBuffer_CBC::daqSourceID() const
  { return daqHeader_.sourceID(); }

  //methods to get info from DAQ trailer uses FEDDAQTrailer class
  inline uint32_t FEDBuffer_CBC::daqEventLengthIn64bitWords() const
  { return daqTrailer_.eventLengthIn64BitWords(); }

  inline uint32_t FEDBuffer_CBC::daqEventLengthInBytes() const
  { return daqTrailer_.eventLengthInBytes(); }

  inline uint16_t FEDBuffer_CBC::daqCRC() const
  { return daqTrailer_.crc(); }

  inline FEDTTSBits FEDBuffer_CBC::daqTTSState() const
  { return daqTrailer_.ttsBits(); }

  // end of FEDBuffer methods definitions }

} // end of sistrip namespace

#endif // } end def EventFilter_SiStripRawToDigi_SiStripFEDBuffer_CBC_H

