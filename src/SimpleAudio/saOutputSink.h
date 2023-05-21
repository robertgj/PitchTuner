/// \file saOutputSink.h
/// \brief Abstract base class for a simple audio output

#include <string>
#include <cstdint>

#include "saDeviceType.h"

#ifndef __SAOUTPUTSINK_H__
#define __SAOUTPUTSINK_H__

namespace SimpleAudio
{
  /// \class saOutputSink
  /// \brief A class for audio output devices
  class saOutputSink
  {
  public:

    /// \typedef saSinkType
    /// The basic type of the output sink
    typedef saDeviceType saSinkType;

    /// Vector of values bound for the output sink
    typedef saSinkType* saOutputFragment;

    saOutputSink() {}

    virtual ~saOutputSink() throw() {}

    /// \brief Claim a fragment buffer from a simple audio output
    /// \return \c saOutputFragment 
    virtual saOutputFragment Claim() = 0;

    /// \brief Dispatch a complete fragment to the audio output sink
    /// \param fragment fragment of frames of samples
    /// \param numValid number of valid frames in the fragment 
    virtual void Dispatch(const saOutputFragment fragment,
                          const std::size_t numValid) = 0;

    /// \brief Clear the sink flags
    virtual void Clear() = 0;

    /// \brief Check for sink under-runs
    /// \return under run occurred
    virtual bool UnderRun() = 0;

    /// \brief Get device frame rate
    /// \return frames per second
    virtual std::size_t GetFramesPerSecond() const = 0;

    /// \brief Get samples per frame
    /// \return samples per frame
    virtual std::size_t GetSamplesPerFrame() const = 0;

    /// \brief Get device frames per fragment
    /// \return frames per fragment
    virtual std::size_t GetFramesPerFragment() const = 0;

    /// \brief Get device frames written to sink
    /// \return frames written to sink
    virtual std::size_t GetFramesWrittenToSink() const = 0;

    /// \brief Get device latency in milliseconds
    /// \return device latency
    virtual std::size_t GetMsDeviceLatency() const = 0;

  protected:

    /// Disable copying an output sink
    saOutputSink(const saOutputSink&);
    saOutputSink& operator =(const saOutputSink&);
  };

  /// \brief Open a device sink
  /// \param deviceName name of device
  /// \param framesPerSecond frame rate
  /// \param msLatency device storage latency in milliseconds
  /// \param channels samples per frame
  /// \return pointer to saOutputSink
  saOutputSink* saOutputSinkOpenDevice(const std::string& deviceName, 
                                       const std::size_t framesPerSecond,
                                       const std::size_t msLatency,
                                       const std::size_t channels = 1 );
  /// \brief Open a file sink
  /// \param fileName name of file
  /// \param framesPerSecond frame rate
  /// \param channels samples per frame
  /// \return pointer to saOutputSink
  saOutputSink* saOutputSinkOpenFile(const std::string& fileName,
                                     const std::size_t framesPerSecond,
                                     const std::size_t channels = 1 );
                                         
}

#endif

