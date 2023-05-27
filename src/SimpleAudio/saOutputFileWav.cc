/// \file saOutputFileWav.cc

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <vector>

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdint>
#include <fcntl.h>

#include "saWavHeader.h"
#include "saOutputSink.h"

namespace SimpleAudio
{
  class saOutputFileWav : public saOutputSink
  {
  public:
    saOutputFileWav(const std::string FileName,
                    const std::size_t framesPerSecond,
                    const std::size_t channels);

    ~saOutputFileWav() throw();

    /// \brief Claim a fragment buffer from a simple audio output
    /// \return saOutputFragment 
    saOutputFragment Claim();

    /// \brief Dispatch a complete fragment to the audio output sink
    /// \param fragment 
    /// \param numValid number of valid frames in the fragment 
    void Dispatch(const saOutputFragment fragment, const std::size_t numValid);

    /// \brief Clear the sink flags
    void Clear( ) {};

    /// \brief Check for sink under runs
    /// \return an under run occurred
    bool UnderRun( ) { return false; }

    /// \brief Get device frame rate
    /// \return frames per second
    std::size_t GetFramesPerSecond() const { return framesPerSecond; }

    /// \brief Get samples per frame
    /// \return samples per frame
    std::size_t GetSamplesPerFrame() const { return samplesPerFrame; }
    
    /// \brief Get device frames per fragment
    /// \return frames per fragment
    std::size_t GetFramesPerFragment() const { return framesPerFragment; }
    
    /// \brief Get device frames written to sink
    /// \return frames written to sink
    std::size_t GetFramesWrittenToSink() const { return framesWrittenToSink; }

    /// \brief Get device frames per sink
    /// \return frames per sink
    std::size_t GetFramesPerSink() const { return 0; }

    /// \brief Get device latency in milliseconds
    /// \return device latency
    std::size_t GetMsDeviceLatency() const 
    { return (1000*framesPerFragment)/framesPerSecond; }

  private:
    /// Sink file name
    std::string sinkName;
        
    /// Frames per second
    std::size_t framesPerSecond;
    
    /// Number of bytes in a sample
    std::size_t bytesPerSample;

    /// Samples per frame
    std::size_t samplesPerFrame;
    
    /// Bytes per frame
    std::size_t bytesPerFrame;
    
    /// Frames per fragment
    std::size_t framesPerFragment;

    /// Samples per fragment
    std::size_t samplesPerFragment;

    /// Bytes per fragment
    [[maybe_unused]] std::size_t bytesPerFragment;

    /// Frames written to sink 
    std::size_t framesWrittenToSink;

    /// Array of samples 
    std::vector<saOutputSink::saSinkType> sinkBuffer;

    /// Output file stream
    std::ofstream outputFile;

    /// Wav file header
    saWavHeader wavHeader;

    /// Wav file header encoder
    void WriteWavFileHeader();
  };

  saOutputFileWav::saOutputFileWav( const std::string fileName,
                                    const std::size_t frameRate,
                                    const std::size_t channels )
    : sinkName( fileName.begin(), fileName.end() ), 
      framesPerSecond( frameRate ),
      bytesPerSample( sizeof(saSinkType) ),
      samplesPerFrame( channels ),
      bytesPerFrame( bytesPerSample*samplesPerFrame ),
      framesPerFragment( 16384 ), // Arbitrary size!
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      bytesPerFragment( bytesPerSample*samplesPerFrame ),
      framesWrittenToSink( 0 ),
      sinkBuffer( samplesPerFragment, 0 ),
      outputFile( sinkName.c_str(), std::ios::binary )
  {
    // Check the input file
    if ( !outputFile.good() )
      {
        throw std::runtime_error("can't open sink file");
      }

    // Write an empty header
    memset(&wavHeader, 0, sizeof(saWavHeader));
    char *buf = reinterpret_cast<char*>(&wavHeader);
    outputFile.write(buf, sizeof(saWavHeader));
    if ( !outputFile.good() ) 
      {
        throw std::runtime_error(" failed writing dummy header to sink file");
      }
  }

  saOutputFileWav::~saOutputFileWav( ) throw()
  { 
    try
      {
        // Write header
        WriteWavFileHeader();

        // Close file
        outputFile.close();
      }
    catch(std::exception& excpt)
      {
        std::cerr << excpt.what() << std::endl ; 
      }
  }

  saOutputSink::saOutputFragment saOutputFileWav::Claim()
  { 
    return &(sinkBuffer[0]);
  }

  void saOutputFileWav::Dispatch(saOutputFragment fragment, 
                                 const std::size_t validFrames)
  { 
    // Swap bytes for big-endian
    if ( !saIsLittleEndian() )
      {
        std::for_each(&(fragment[0]),
                      &(fragment[samplesPerFrame*validFrames]),
                      saSwapSample<saSinkType>());
      }

    // Write
    char* buf = reinterpret_cast<char*>(&(fragment)[0]);
    outputFile.write(buf,
                     static_cast<std::streamsize>(validFrames*bytesPerFrame));
    if (!outputFile.good())
      {
        throw std::runtime_error(" failed writing fragment");        
      }
    framesWrittenToSink += validFrames;

    return;
  }

  void saOutputFileWav::WriteWavFileHeader()
  {
    // Fill in sink parameters

    // Format
    memcpy(&(wavHeader.FormatChunkID), "fmt ", 4);
    wavHeader.FormatChunkSize = 16;
    wavHeader.FormatTag = 1;
    wavHeader.FormatChannels = (short unsigned int)samplesPerFrame;
    wavHeader.FormatBlockAlign = (short unsigned int)bytesPerFrame;
    wavHeader.FormatAvgBytesPerSec =
      wavHeader.FormatBlockAlign*((unsigned int)framesPerSecond);
    wavHeader.FormatSamplesPerSec = (short unsigned int)framesPerSecond;
    wavHeader.FormatBitsPerSample = (short unsigned int)(bytesPerSample*8);

    // Data
    memcpy(&(wavHeader.DataChunkID), "data", 4);  
    wavHeader.DataChunkSize =
      wavHeader.FormatBlockAlign*(int)framesWrittenToSink;

    // RIFF
    memcpy(&(wavHeader.RiffChunkID), "RIFF", 4);
    wavHeader.RiffChunkSize = 4+(8+wavHeader.FormatChunkSize)+
      (8+wavHeader.DataChunkSize);
    memcpy(&(wavHeader.RiffFormat), "WAVE", 4);

    // Big-endian?
    if ( !saIsLittleEndian() )
      {
        saSwapWavHeader(&wavHeader);
      }

    // Rewind output file
    outputFile.seekp(std::ios_base::beg);
    if ( !outputFile.good() ) 
      {
        throw std::runtime_error(" failed seeking to start of sink file");
      }

    // Write output file header
    char *buf = reinterpret_cast<char*>(&wavHeader);
    outputFile.write(buf, sizeof(saWavHeader));
    if ( !outputFile.good() ) 
      {
        throw std::runtime_error(" failed writing wav header to sink file");
      }
  }

  saOutputSink* saOutputSinkOpenFileWav( const std::string& fileName,
                                         const std::size_t framesPerSecond,
                                         const std::size_t channels = 1 )
  {
    return new saOutputFileWav(fileName, framesPerSecond, channels);
  }

}
