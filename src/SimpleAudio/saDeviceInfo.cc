// saDeviceInfo.cc

#include <iostream>
#include <iomanip>
#include <iosfwd>
#include <iterator>
#include <algorithm>
#include <utility>
#include <stdexcept>

#include "saDeviceInfo.h"

namespace SimpleAudio
{
  void 
  saDeviceInfo::getDeviceNames(saDeviceInfoDeviceNameList& deviceNameList)
  {
    deviceNameList.clear();
    saDeviceInfoList::iterator info;
    for( info = deviceInfoList.begin(); info != deviceInfoList.end(); info++)
      {
        saDeviceInfoListEntry entry = *info;
        deviceNameList.push_back(entry.first);
      }
  }

  void
  saDeviceInfo::getDeviceSampleRateList (saDeviceInfoDeviceName& deviceName, 
                                         saDeviceInfoSampleRateList& 
                                         sampleRateList)
  {
    saDeviceInfoList::iterator info;
    for(info = deviceInfoList.begin(); info != deviceInfoList.end(); info++)
      {
        saDeviceInfoListEntry entry = *info;

        if (entry.first == deviceName)
          {
            sampleRateList.clear();
            sampleRateList = entry.second;
            break;
          }
      }
  }

  // Functor object echoing simple audio device info
  struct saDeviceInfo::showListEntry
  {
    void operator() (const saDeviceInfoListEntry& entry)
    {
      // Device name
      std::cerr << entry.first << std::endl;
        
      // Device sample rates
      saDeviceInfoSampleRateList sampleRates = entry.second;
      std::ostream_iterator<saSampleRate> output(std::cerr, " ");
      std::copy(sampleRates.begin(), sampleRates.end(), output);
        
      // Done
      std::cerr << std::endl;
    }
  };

  bool
  saDeviceInfo::Empty()
  {
    return deviceInfoList.empty(); 
  }

  void 
  saDeviceInfo::showDevices()
  {
    std::for_each(deviceInfoList.begin(), 
                  deviceInfoList.end(), 
                  showListEntry());
  }

}  // namespace SimpleAudio
