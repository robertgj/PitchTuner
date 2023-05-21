/// \file saInputStream.h
/// \brief Abstract class for a simple audio input stream

#include <vector>

#include "saSample.h"

#ifndef __SAINPUTSTREAM_H__
#define __SAINPUTSTREAM_H__

namespace SimpleAudio
{
  /// \class saInputStream
  /// \brief A class for audio input streams from a file, audio device etc.
  class saInputStream
  {
  public:

    saInputStream() { }

    virtual ~saInputStream() throw() { }

    /// \brief Read a buffer of frames from the saInputStream
    /// \param buffer vector< saSample > 
    /// \param framesToRead
    /// \return number of frames read
    virtual std::size_t Read( std::vector< saSample >& buffer,
                              const std::size_t framesToRead) = 0;

    /// \brief Read a buffer of samples from the saInputStream
    /// \param buffer vector of saSample
    /// \param framesToRead frames to read 
    /// \param sampleInFrame read sample in frame
    /// \return number of frames read
    virtual std::size_t Read( std::vector< saSample >& buffer,
                              const std::size_t framesToRead,
                              const std::size_t sampleInFrame) = 0;

    /// \brief Clear the stream
    virtual void Clear( ) = 0;

    /// \brief Check for stream over runs
    /// \return over run occurred
    virtual bool OverRun( ) = 0;

    /// \brief Check for stream end of source
    /// \return end of source occurred
    virtual bool EndOfSource( ) = 0;

    /// \brief Get device frame rate
    /// \return frames per second
    virtual std::size_t GetFramesPerSecond() const = 0;

    /// \brief Get device samples per frame
    /// \return samples per frame
    virtual std::size_t GetSamplesPerFrame() const = 0;

    /// \brief Get device frames per fragment
    /// \return frames per fragment
    virtual std::size_t GetFramesPerFragment() const = 0;

    /// \brief Get capacity of the stream
    /// \return frames per stream
    virtual std::size_t GetFramesPerStream() const = 0;

    /// \brief Get frames read from the stream
    /// \return frames read from stream
    virtual std::size_t GetFramesReadFromStream() const = 0;

  protected:
    /// Disallow assignment to saInputStream
    saInputStream& operator=( const saInputStream& );

    /// Disallow copy constructor of saInputStream
    saInputStream( const saInputStream& );
  };

  /// \brief Open a simple audio input stream from a device
  /// \param deviceName device name
  /// \param framesPerSecond sample rate
  /// \param msDeviceLatency device fragment buffer size (stream latency in ms)
  /// \param channels number of channels on device
  /// \param debug
  /// \return pointer to saInputStream
  saInputStream* saInputStreamOpenDevice( const std::string& deviceName, 
                                          const std::size_t framesPerSecond,
                                          const std::size_t msDeviceLatency,
                                          const std::size_t channels=1,
                                          const bool debug=false );

  /// \brief Open a simple audio input stream from a file
  /// \param fileName file name
  /// \return pointer to saInputStream 
  saInputStream* saInputStreamOpenFile( const std::string& fileName );

}

#endif
