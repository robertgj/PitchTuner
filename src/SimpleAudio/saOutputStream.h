/// \file saOutputStream.h
/// \brief Abstract class for a simple audio output stream. 

#include <vector>

#include "saSample.h"

#ifndef __SAOUTPUTSTREAM_H__
#define __SAOUTPUTSTREAM_H__

namespace SimpleAudio
{
  /// \class saOutputStream
  /// \brief A class for audio output streams to a file, audio device etc.
  class saOutputStream
  {
  public:

    saOutputStream() { }

    virtual ~saOutputStream() throw() { }

    /// \brief Write a buffer of frames to the saOutputStream
    /// \param buffer vector of saSample
    /// \param framesToWrite frames to write
    /// \return number of frames written
    virtual std::size_t Write( std::vector< saSample >& buffer,
                               const std::size_t framesToWrite = 1 ) = 0;

    /// \brief Check for stream under runs
    /// \return stream under run occurred
    virtual bool UnderRun( ) = 0;

    /// \brief Get device frame rate
    /// \return frames per second
    virtual std::size_t GetFramesPerSecond() const = 0;

    /// \brief Get device samples per frame
    /// \return samples per frame
    virtual std::size_t GetSamplesPerFrame() const = 0;

    /// \brief Get device frames per fragment
    /// \return frames per fragment
    virtual std::size_t GetFramesPerFragment() const = 0;

    /// \brief Get frames written to the stream
    /// \return frames written to stream
    virtual std::size_t GetFramesWrittenToStream() const = 0;

  protected:
    /// Disallow assignment to saOutputStream
    saOutputStream& operator=( const saOutputStream& );

    /// Disallow copy constructor of saOutputStream
    saOutputStream( const saOutputStream& );
  };

  /// \brief Open a simple audio output stream to a device
  /// \param deviceName name of device
  /// \param framesPerSecond sample rate
  /// \param msDeviceLatency device fragment buffer size (stream latency in ms)
  /// \param channels channels (samples) per fragment
  /// \return pointer to saOutputStream
  saOutputStream* saOutputStreamOpenDevice( const std::string& deviceName, 
                                            const std::size_t framesPerSecond,
                                            const std::size_t msDeviceLatency,
                                            const std::size_t channels = 1 );

  /// \brief Open a simple audio output stream to a file
  /// \param fileName name of file
  /// \param framesPerSecond frame (sample) rate
  /// \param channels samples per frame
  /// \return pointer saOutputStream
  saOutputStream* saOutputStreamOpenFile( const std::string& fileName,
                                          const std::size_t framesPerSecond,
                                          const std::size_t channels = 1 );

}

#endif
