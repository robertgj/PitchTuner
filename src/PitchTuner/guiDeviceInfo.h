/// \file guiDeviceInfo.h
///
//  List of available devices for the gui
//

#include <stdexcept>
#include <iostream>
#include <vector>

#include <wx/wx.h>

#ifndef __GUIDEVICEINFO_H__
#define __GUIDEVICEINFO_H__

namespace PitchTunerGui
{
  typedef std::vector<std::size_t> guiSampleRateList;

  /// \class guiDeviceInfo
  /// A class for defining audio device information in the wxWidgets GUI options
  /// dialog box.
  class guiDeviceInfo
  {
    /// Declare list of sample rate lists
  public:

    guiDeviceInfo(std::size_t numDevices = 4) : 
      m_deviceNameList(), 
      m_sampleRateLists(), 
      m_numDevicesAlloced(numDevices),
      m_numDevices(0)
    { 
      m_deviceNameList.Alloc(m_numDevicesAlloced);
      m_sampleRateLists.reserve(m_numDevicesAlloced);
    }
    
    ~guiDeviceInfo() {};

    /// \brief Alloc device storage
    /// \param numDevices number of devices 
    void Alloc(const std::size_t numDevices)
    {
      if (numDevices > m_numDevicesAlloced)
        {
          m_deviceNameList.Alloc(numDevices);
          m_sampleRateLists.reserve(numDevices);
          m_numDevicesAlloced = numDevices;
        }
    }

    /// \brief Add device
    /// \param deviceName reference to a string
    /// \param rates reference to a vector of ints
    void AddDevice(const std::string deviceName, 
                   const guiSampleRateList& rates)
    {
      if (m_numDevices >= m_numDevicesAlloced)
        {
          Alloc(m_numDevicesAlloced*2);
        }
      m_deviceNameList.Add(wxString(deviceName));
      m_sampleRateLists.push_back(rates);
      m_numDevices++;
    }

    /// \brief Get number of devices
    ///  \return std::size_t
    std::size_t GetCount() const
    {
      return m_deviceNameList.GetCount();
    }

    /// \brief Get device names
    /// \param deviceNameList reference to a wxArrayString
    void GetDeviceNameList(wxArrayString& deviceNameList) const
    {
      deviceNameList.Alloc(m_numDevices);
      deviceNameList = m_deviceNameList;
    }

    /// \brief Get device sample rates
    /// \param reqDeviceName references to device name requested
    /// \param rates reference to a vector of sample rates
    void GetDeviceSampleRateList(const wxString& reqDeviceName, 
                                 guiSampleRateList& rates) const
    {
      int deviceNum = m_deviceNameList.Index(reqDeviceName);
      if (deviceNum == wxNOT_FOUND)
        {
          throw std::runtime_error("Can't find device");
        }
      rates.assign(m_sampleRateLists[deviceNum].begin(), 
                   m_sampleRateLists[deviceNum].end());
      return;
    }
    
    /// \brief Get device name
    /// \param deviceNameIndex 
    /// \return reference to deviceName
    const wxString& GetDeviceName(const std::size_t deviceNameIndex) const
    {
      if (deviceNameIndex >= m_numDevices)
        {
          throw std::runtime_error("Invalid device name index");
        }
      return m_deviceNameList[deviceNameIndex];
    }

    /// \brief Get device sample rate
    /// \param deviceNameIndex 
    /// \param sampleRateIndex 
    /// \return sample rate
    std::size_t GetDeviceSampleRate(const std::size_t deviceNameIndex,
                                    const std::size_t sampleRateIndex) const
    {
      if (deviceNameIndex >= m_numDevices)
        {
          throw std::runtime_error("Invalid device name index");
        }
      std::size_t numRates = m_sampleRateLists[deviceNameIndex].size();
      if (sampleRateIndex >= numRates)
        {
          throw std::runtime_error("Invalid sample rate index");
        }
      return m_sampleRateLists[deviceNameIndex][sampleRateIndex];
    }
    
    /// \brief Device info list empty?
    /// \return device is empty
    bool IsEmpty(){ return (m_numDevices == 0); }


    /// \brief Show device info
    void ShowDevices()
    {
      for(std::size_t deviceNum=0; deviceNum<m_numDevices; deviceNum++)
        {
          std::cout << m_deviceNameList[deviceNum].c_str() << " : ";
          std::size_t numRates = m_sampleRateLists[deviceNum].size();
          for(std::size_t rateNum=0; rateNum<numRates; rateNum++)
            {
              std::cout << " " << m_sampleRateLists[deviceNum][rateNum];
            }
          std::cout << std::endl;
        }
    }

  private:
    /// Internal list of available devices
    wxArrayString m_deviceNameList;

    /// Internal list of lists of sample rates for each available device
    std::vector<guiSampleRateList> m_sampleRateLists;

    /// Number of pre-allocated devices
    std::size_t m_numDevicesAlloced;

    /// Number of devices
    std::size_t m_numDevices;

    /// Disallow assignment to guiDeviceInfo
    guiDeviceInfo& operator=(const guiDeviceInfo&);

    /// Disallow copy constructor of guiDeviceInfo
    guiDeviceInfo(const guiDeviceInfo&);
  };

}

#endif
