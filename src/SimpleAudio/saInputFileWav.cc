/// \file saInputFileWav.cc
/// \brief WAV file input

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include <cstring>
#include <cstdint>

#include "saWavHeader.h"
#include "saInputSource.h"

namespace SimpleAudio
{
  class saInputFileWav : public saInputSource
  {
  public:
    saInputFileWav(const std::string& FileName, saWavHeader * const wavHeader);

    ~saInputFileWav() throw();

    /// \brief Read a fragment from a simple audio input
    /// \param validFrames number of valid frames in the fragment
    /// \return saFragment 
    saInputFragment Collect(std::size_t& validFrames);

    /// \brief Release fragment storage
    /// \param fragment 
    void Release([[maybe_unused]] saInputFragment fragment) { } 

    /// \brief Clear the source
    void Clear( ) {};

    /// \brief Check for source over-runs
    /// \return source over run occurred
    bool OverRun( ) { return false; }

    /// \brief Check a simple audio input file for end-of-data
    ///  (Only makes sense for files).
    /// \return end of source occurred
    bool EndOfSource() { return inputFile.eof(); }

    /// \brief Get device frame rate
    /// \return frames per second
    std::size_t GetFramesPerSecond() const { return framesPerSecond; }

    /// \brief Get samples per frame
    ///\return samples per frame
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
        
    /// Input file stream
    std::ifstream inputFile;

    /// Frames per second
    std::size_t framesPerSecond;
    
    /// Number of bytes in a sample
    std::size_t bytesPerSample;

    /// Samples per frame
    std::size_t samplesPerFrame;
    
    /// Frames per fragment
    std::size_t framesPerFragment;

    /// Samples per fragment
    std::size_t samplesPerFragment;

    /// Bytes per fragment
    std::size_t bytesPerFragment;

    /// Maximum number of bytes stored in the source
    std::size_t bytesPerSource;

    /// Frames per source
    std::size_t framesPerSource;

    /// Frames read from source 
    std::size_t framesReadFromSource;

    /// Array of samples 
    std::vector< saInputSource::saSourceType > sourceBuffer;
  };

  saInputFileWav::saInputFileWav(const std::string& fileName,
                                 saWavHeader * const wavHeader)
    : sourceName( fileName.begin(), fileName.end() ), 
      inputFile(sourceName.c_str(), std::ios::binary),
      framesPerSecond( wavHeader->FormatSamplesPerSec ),
      bytesPerSample( wavHeader->FormatBitsPerSample/8 ),
      samplesPerFrame( wavHeader->FormatChannels ),
      framesPerFragment( 16384 ), // Arbitrary size!
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      bytesPerFragment( bytesPerSample*samplesPerFragment ),
      framesPerSource( static_cast<std::size_t>(wavHeader->DataChunkSize) /
                       (bytesPerSample*samplesPerFrame) ),
      framesReadFromSource( 0 ),
      sourceBuffer( samplesPerFragment, 0 )
  {
    // Read the header
    inputFile.read((char *)wavHeader, sizeof(saWavHeader));
    if ( !inputFile.good() ) 
      {
        throw std::runtime_error(" failed read wav header from source file");
      }
  }

  saInputFileWav::~saInputFileWav( ) throw()
  { 
    // Close file
    inputFile.close();
  }

  saInputSource::saInputFragment 
  saInputFileWav::Collect(std::size_t& framesRead)
  { 
    // Read
    char* buf = reinterpret_cast<char*>(&(sourceBuffer[0]));
    inputFile.read(buf, static_cast<long>(bytesPerFragment));
    if ( !inputFile.good() && !inputFile.eof() ) 
      {
        throw std::runtime_error("read from file failed");
      }

    // Zero out bytes in fragment not filled
    std::size_t bytesRead = static_cast<std::size_t>(inputFile.gcount());
    std::size_t samplesRead = bytesRead/static_cast<std::size_t>(bytesPerSample);
    if ( samplesRead < samplesPerFragment )
      {
        memset(&(sourceBuffer[samplesRead]), 0, bytesPerFragment-bytesRead);
      }
  
    // Swap bytes for big-endian
    if ( !saIsLittleEndian() )
      {
        std::for_each(sourceBuffer.begin(), 
                      sourceBuffer.begin()+static_cast<long>(samplesRead), 
                      saSwapSample<saInputSource::saSourceType>());
      }

    // Set frames read
    framesRead = samplesRead/samplesPerFrame;
    framesReadFromSource += framesRead;

    // Done
    return &(sourceBuffer[0]);
  }

  saInputSource* saInputSourceOpenFileWav(const std::string& fileName)
  {
    // Open the input file
    std::string name(fileName.begin(), fileName.end());
    std::ifstream inputFile(name.c_str(), std::ios::binary);
    if ( !inputFile.good() )
      {
        throw std::runtime_error("can't open source file");
      }

    // Read the header
    saWavHeader wavHeader;
    inputFile.read((char*)&wavHeader, sizeof(saWavHeader));
    if ( !inputFile.good() ) 
      {
        throw std::runtime_error(" failed read wav header from source file");
      }
    if ( !saIsLittleEndian() )
      {
        saSwapWavHeader(&wavHeader);
      }

    // Close the file
    inputFile.close();

    // Sanity check on IDs
    if ( ! (saCompareID(wavHeader.RiffChunkID, "RIFF") &&
            saCompareID(wavHeader.RiffFormat, "WAVE") &&
            saCompareID(wavHeader.FormatChunkID, "fmt ") &&
            saCompareID(wavHeader.DataChunkID, "data")))
      {
        throw std::runtime_error("id failed in source file");
      }
    
    // Sanity check on content
    if (wavHeader.FormatTag != 1)
      {
        throw std::runtime_error("compression not supported");
      }

    // Sanity check
    if ((wavHeader.FormatChannels*wavHeader.FormatBitsPerSample/8) >= 
        wavHeader.DataChunkSize/2)
      {
        std::cerr << "Sanity check: Did you expect "
                  << wavHeader.FormatChannels 
                  << " channels per frame?" 
                  << std::endl;
      }

    // Select source type
    switch (wavHeader.FormatBitsPerSample)
      {
      case 16:
        return new saInputFileWav(fileName, &wavHeader);
        break;

      default:
        throw std::runtime_error("source bits per sample not supported");
        break;
      }

    // Done
    return 0;
  }

}
