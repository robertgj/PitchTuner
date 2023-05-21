// \file saInputDeviceInfo_test.cc
//

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "saDeviceInfo.h"

using namespace SimpleAudio;

int
main(void)
{
  saDeviceInfo deviceInfo;
  try
    {
      deviceInfo.queryInputDevices();
    }
  catch(std::exception& excp)
    {
      std::cerr << excp.what() << std::endl ;
    }

  if (deviceInfo.Empty())
    {
      std::cerr << "No audio input devices found!" << std::endl ;
    }
  else
    {
      deviceInfo.showDevices();
    }

  saDeviceInfoDeviceNameList deviceNameList;
  deviceInfo.getDeviceNames(deviceNameList);
  for (saDeviceInfoDeviceNameList::iterator name = deviceNameList.begin();
       name != deviceNameList.end();
       name++)
    {
      std::cerr << "Device: " << *name << std::endl ;
      saDeviceInfoSampleRateList sampleRateList;
      deviceInfo.getDeviceSampleRateList(*name, sampleRateList);
      std::cerr << "Sample rates: " ;
      for(saDeviceInfoSampleRateList::iterator rate = sampleRateList.begin();
          rate != sampleRateList.end(); 
          rate++)
        {
          std::cerr << *rate << " " ;
        }
      std::cerr << std::endl ;
    }

  return 0;
}
