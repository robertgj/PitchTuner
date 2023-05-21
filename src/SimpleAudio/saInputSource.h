/// \file saInputSource.h
/// \brief Abstract base class for a simple audio input. 

#include <cstdint>

#include "saDeviceType.h"

#ifndef __SAINPUTSOURCE_H__
#define __SAINPUTSOURCE_H__

namespace SimpleAudio
{
  /// \class saInputSource
  /// \brief A class for audio input devices
  class saInputSource
  {
  public:

    /// \typedef saSourceType
    /// The basic type of the input source
    typedef saDeviceType saSourceType;

    /// Pointer to an array of values from the input source.
    typedef saSourceType* saInputFragment;

    saInputSource() { } ;

    virtual ~saInputSource() throw() {};

    /// \brief Collect a fragment from a simple audio input queue
    ///
    /// Collect a fragment from a simple audio input queue
    /// The user shouldn't be able to modify an saFragment 
    /// returned by this function.
    /// \param numValid number of valid frames in the fragment
    /// \return saInputFragment 
    virtual saInputFragment Collect(std::size_t& numValid) = 0;

    /// \brief Release fragment storage to the simple audio input queue
    /// \param fragment 
    virtual void Release(const saInputFragment fragment) = 0;

    /// \brief Clear the source flags
    virtual void Clear( ) = 0;

    /// \brief Check for source over-runs
    /// \return source over run occurred
    virtual bool OverRun( ) = 0;

    /// \brief Check a simple audio input source for end-of-data
    ///        (Only makes sense for files).
    /// \return end of source occurred
    virtual bool EndOfSource() = 0;

    /// \brief Get device frame rate
    /// \return frames per second
    virtual std::size_t GetFramesPerSecond() const = 0;

    /// \brief Get samples per frame
    /// \return samples per frame
    virtual std::size_t GetSamplesPerFrame() const = 0;

    /// \brief Get device frames per fragment
    /// \return frames per fragment
    virtual std::size_t GetFramesPerFragment() const = 0;

    /// \brief Get device frames per source
    /// \return frames per source
    virtual std::size_t GetFramesPerSource() const = 0;

    /// \brief Get device frames read from source
    /// \return frames read from source
    virtual std::size_t GetFramesReadFromSource() const = 0;

    /// \brief Get device latency in milliseconds
    /// \return device latency
    virtual std::size_t GetMsDeviceLatency() const = 0;

  protected:

    saInputSource(const saInputSource&);
    saInputSource& operator =(const saInputSource&);
    
  };
  
  /// saInputSourceOpenDevice
  /// \param deviceName Name of audio input device to open
  /// \param framesPerSecond Sample rate to set in the audio device
  /// \param msLatency Size of buffer of the device data in milliseconds
  /// \param channels Channels per frame of the device
  /// \param debug Enable debugging output
  /// \return A pointer to an saInputSource
  saInputSource* saInputSourceOpenDevice(const std::string& deviceName, 
                                         const std::size_t framesPerSecond,
                                         const std::size_t msLatency,
                                         const std::size_t channels = 1,
                                         const bool debug = false);

  saInputSource* saInputSourceOpenFile(const std::string& fileName); 

}

#endif

