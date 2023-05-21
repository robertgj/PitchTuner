// saDeviceInfoDetail.cc
// Implementation of simple audio interface device query.
// This version is for ALSA and simply returns default values.

// StdC++ headers
#include <iostream>
#include <iomanip>
#include <iosfwd>
#include <iterator>
#include <stdexcept>

// C library headers
#include <cstdio>
#include <cerrno>
#include <fcntl.h>

// C system headers
#include <sys/ioctl.h>
#include <sys/soundcard.h>

// Local headers
#include "saDeviceInfo.h"

namespace SimpleAudio
{
  void 
  saDeviceInfo::queryInputDevices()
  {
    saDeviceInfoSampleRateList sampleRatesList;
    sampleRatesList.push_back(48000);
    sampleRatesList.push_back(44100);
    sampleRatesList.push_back(22050);
    sampleRatesList.push_back(11025);

    std::string deviceName("default");
    deviceInfoList.clear();
    deviceInfoList.push_back(make_pair(deviceName, sampleRatesList));
  }

}
