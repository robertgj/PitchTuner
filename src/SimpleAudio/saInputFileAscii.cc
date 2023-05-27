/// \file saInputFileAscii.cc
/// \brief ASCII file input
///
/// ASCII file reader for SimpleAudio. The first three lines are 
/// assumed to be framesPerSecond, samplesPerFrame and framesPerStream.
/// One sample per line. Samples are in a format compatible with conversion
/// to the saSourceType

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdexcept>

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdint>
#include <fcntl.h>

#include "saInputSource.h"

namespace SimpleAudio
{
  template<typename T>
  struct ReadFromFile
  {
    ReadFromFile(const char * source_name) 
    {
      infile.open(source_name);
    }

    ~ReadFromFile() 
    {
      infile.close();
    }

    bool is_ok(void)
    {
      return infile.good();
    }
    
    bool end_of_file(void)
    {
      return infile.eof();
    }
    
    bool read_header(std::size_t *header) 
    {
      header[0] = *std::istream_iterator<std::size_t>(infile);
      header[1] = *std::istream_iterator<std::size_t>(infile);
      header[2] = *std::istream_iterator<std::size_t>(infile);
      return infile.good();
    }
      
    T read()
    {
      return *std::istream_iterator<T>(infile);
    }
  private:
    std::ifstream infile;
  };
  
  class saInputFileAscii : public saInputSource
  {
  public:
    saInputFileAscii(const std::string FileName,
                     const std::size_t _framesPerSecond,
                     const std::size_t _samplesPerFrame,
                     const std::size_t _framesPerSource);

    ~saInputFileAscii() throw();

    /// \brief Read a fragment from a simple audio input
    /// \param validFrames number of valid frames in the fragment
    /// \return saInputFragment 
    saInputFragment Collect(std::size_t& validFrames);

    /// \brief Release fragment storage
    /// \param fragment 
    void Release([[maybe_unused]] saInputFragment fragment) { } 

    /// \brief Clear the source
    void Clear( ) { };

    /// \brief Check for source over-runs
    /// \return device over run occurred
    bool OverRun( ) { return false; }

    /// \brief Check a simple audio input file for end-of-data
    /// (Only makes sense for files).
    //  \return end of source found
    bool EndOfSource() { return rff.end_of_file(); }

    /// \brief Get device frame rate
    /// \return frams per second
    std::size_t GetFramesPerSecond() const { return framesPerSecond; }

    /// \brief Get samples per frame
    /// \return samples per frame
    std::size_t GetSamplesPerFrame() const { return samplesPerFrame; }
    
    /// \brief Get device frames per fragment
    /// \return frames per fragment
    std::size_t GetFramesPerFragment() const { return framesPerFragment; }
    
    /// \brief Get device frames per source
    /// \return frames per source
    std::size_t GetFramesPerSource() const { return framesPerSource; }

    /// \brief Get device frames read from source
    /// \return frames read from source
    std::size_t GetFramesReadFromSource() const { return framesReadFromSource; }

    /// \brief Get device latency in milliseconds
    /// \return device latency
    std::size_t GetMsDeviceLatency() const 
    { return (1000*framesPerFragment)/framesPerSecond; }

  private:

    /// Source device name
    std::string sourceName;
        
    /// Frames per second
    std::size_t framesPerSecond;
    
    /// Samples per frame
    std::size_t samplesPerFrame;
    
    /// Frames per fragment
    std::size_t framesPerFragment;

    /// Samples per fragment
    std::size_t samplesPerFragment;

    /// Bytes per fragment
    [[maybe_unused]] std::size_t bytesPerFragment;

    /// Maximum number of bytes stored in the source
    [[maybe_unused]] std::size_t bytesPerSource;

    /// Frames per source
    std::size_t framesPerSource;

    /// Frames read from source 
    std::size_t framesReadFromSource;

    /// Array of samples 
    std::vector<saSourceType> sourceBuffer;

    /// File handle 
    struct ReadFromFile<saSourceType> rff;
  };

  saInputFileAscii::saInputFileAscii(const std::string fileName, 
                                     const std::size_t _framesPerSecond,
                                     const std::size_t _samplesPerFrame,
                                     const std::size_t _framesPerSource)
    : sourceName( fileName.begin(), fileName.end() ), 
      framesPerSecond( _framesPerSecond ),
      samplesPerFrame( _samplesPerFrame ),
      framesPerFragment( 16384 ), // Arbitrary size!
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      framesPerSource( _framesPerSource ),
      framesReadFromSource( 0 ),
      sourceBuffer( samplesPerFragment ),
      rff( sourceName.c_str() )
  {
    if ( !rff.is_ok() )
      {
        throw std::runtime_error("can't open source file");
      }

    // Read past the header
    std::size_t header[3];
    if(!rff.read_header(header))
      {
        throw std::runtime_error("read of header failed");
      }

    // Sanity check
    if((header[0] != framesPerSecond) ||
       (header[1] != samplesPerFrame) ||
       (header[2] != framesPerSource))
      {
        throw std::runtime_error("inconsistent header");
      }
  }

  saInputFileAscii::~saInputFileAscii( ) throw()
  { 
  }

  saInputSource::saInputFragment 
  saInputFileAscii::Collect(std::size_t& framesRead)
  { 
    // Initialise
    std::size_t framesRemainingInSource = framesPerSource-framesReadFromSource;
    std::size_t framesToRead = framesRemainingInSource < framesPerFragment ?
      framesRemainingInSource : framesPerFragment;
    std::size_t samplesToRead = samplesPerFrame*framesToRead;


    // Read
    sourceBuffer.resize(0);
    for (std::size_t i = 0; i < samplesToRead; i++)
      {
        sourceBuffer[i] = rff.read();
      }

    // Check
    if (!rff.is_ok())
      {
        framesRemainingInSource = 0;
        throw std::runtime_error("read from file failed");
      }
    
    // Set frames read
    framesReadFromSource += framesToRead;
    framesRead = framesToRead;

    // Done
    return (framesRead > 0 ? &(sourceBuffer[0]) : 0);
  }

  saInputSource* saInputSourceOpenFileAscii(const std::string& fileName)
  {
    // Open the input file
    std::string name( fileName.begin(), fileName.end() );
    std::ifstream inputFile( name.c_str() );
    if ( !inputFile.good() )
      {
        throw std::runtime_error("can't open source file");
      }

    // Read the header
    std::size_t header[3];
    for (std::size_t i = 0; i < 3; i++)
      {
        header[i] = *std::istream_iterator<std::size_t>(inputFile);
        if ( inputFile.fail() )
          {
            throw std::runtime_error("header read failed");
          }    
      }
    inputFile.close();

    return new saInputFileAscii(fileName, header[0], header[1], header[2]);
  }

}


