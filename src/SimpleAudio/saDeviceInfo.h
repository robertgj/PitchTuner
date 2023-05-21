/// \file saDeviceInfo.h
/// \brief Find available audio devices
///
///  Public interface for finding out about available system audio
///  devices. The only devices SimpleAudio is interested in are mono, 
///  16-bit devices supporting the sample rates 11.025kHz, 22.05kHz, 
///  44.1kHz and/or 48kHz.

#include <stdexcept>
#include <utility>
#include <vector>
#include <string>

#ifndef __SADEVICEINFO_H__
#define __SADEVICEINFO_H__

namespace SimpleAudio
{
  /// \typedef saDeviceInfoDeviceName
  /// Device name
  typedef std::string saDeviceInfoDeviceName;

  /// \typedef saDeviceInfoDeviceNameList
  /// List (vector for random access) of device names
  typedef std::vector< saDeviceInfoDeviceName > saDeviceInfoDeviceNameList;

  /// \typedef saSampleRate
  /// Type of sample rate
  typedef std::size_t saSampleRate;

  /// \typedef saSampleRate
  /// List (vector for random access) of sample rates 
  typedef std::vector< saSampleRate > saDeviceInfoSampleRateList;

  /// \class saDeviceInfo
  /// \brief A class for obtaining information about an audio device
  class saDeviceInfo
  {
  public:

    saDeviceInfo() : deviceInfoList() { } // Stops a compiler warning!

    ~saDeviceInfo() { }

    /// \brief Query simple audio input device info
    void queryInputDevices();

    /// \brief Get device names
    /// \param deviceNameList reference to a vector of device names
    void getDeviceNames(saDeviceInfoDeviceNameList& deviceNameList);

    /// \brief Get device sample rates
    /// \param deviceName
    /// \param sampleRateList reference to a vector of sample rates
    void getDeviceSampleRateList(saDeviceInfoDeviceName& deviceName, 
                                 saDeviceInfoSampleRateList& sampleRateList);

    /// \brief Device info list empty?
    ///  \return Device list is empty
    bool Empty();

    /// \brief Show device info
    void showDevices();

  private:

    /// Disallow assignment to saDeviceInfo
    saDeviceInfo& operator=( const saDeviceInfo& );

    /// Disallow copy constructor of saDeviceInfo
    saDeviceInfo( const saDeviceInfo& );

    /// \typedef saDeviceInfoListEntry
    /// Pair of device name and vector of sample rates
    typedef 
      std::pair< saDeviceInfoDeviceName, saDeviceInfoSampleRateList > 
        saDeviceInfoListEntry;

    /// \typedef saDeviceInfoList
    /// List (vector) of pairs of device name and vector of sample rates
    typedef std::vector< saDeviceInfoListEntry > saDeviceInfoList;

    /// Internal list of available devices
    saDeviceInfoList deviceInfoList;

    /// \struct showListEntry
    /// Show helper function object
    struct showListEntry;
  };

}

#endif
