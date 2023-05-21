/// \file saOutputStream.cc
/// \brief Implementation of a simple audio output stream

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>

#include "saSample.h"
#include "saOutputSink.h"
#include "saOutputStream.h"

namespace SimpleAudio
{
  class saOutputStreamImpl : public saOutputStream
  {
  public:
    saOutputStreamImpl( saOutputSink* sink );

    ~saOutputStreamImpl( ) throw();

    /// \brief Write a buffer of frames to the saOutputStream
    /// \param buffer vector of saSample
    /// \param framesToWrite frames to write to the output stream
    /// \return number of frames written
    std::size_t Write( std::vector< saSample >& buffer,
                       const std::size_t framesToWrite = 1 );

    /// \brief Check for stream under runs
    /// \return stream under run occurred
    bool UnderRun( ) 
    { 
      return outputSink->UnderRun();
    } 

    /// \brief Get device frame rate
    /// \return frame rate
    std::size_t GetFramesPerSecond() const
    { 
      return framesPerSecond; 
    }

    /// \brief Get samples per frame
    /// \return samples per frame
    std::size_t GetSamplesPerFrame() const 
    { 
      return samplesPerFrame; 
    }

    /// \brief Get device frames per fragment
    /// \return frames per fragment
    std::size_t GetFramesPerFragment() const
    { 
      return framesPerFragment; 
    }

    /// \brief Get frames written to the stream
    /// \return frames written to stream
    std::size_t GetFramesWrittenToStream() const 
    { 
      return framesWrittenToStream; 
    }

  private:

    /// Output sink
    std::unique_ptr<saOutputSink> outputSink;

    /// Flag source over-run
    bool isUnderRun;

    /// Number frames per second
    std::size_t framesPerSecond;

    /// Number of samples in a frame
    std::size_t samplesPerFrame;

    /// Number of frames in a fragment
    std::size_t framesPerFragment;

    /// Number of samples in a fragment
    std::size_t samplesPerFragment;

    /// Maximum number of frames in the deque
    std::size_t framesPerStream;

    /// Frames written to stream
    std::size_t framesWrittenToStream;

    /// Frames written to fragment
    std::size_t framesWrittenToFragment;

    /// Frames space remaining in fragment
    std::size_t framesSpaceRemainingInFragment;

    /// Current fragment
    saOutputSink::saSinkType* currentFragment;

    /// Disallow copying saOutputStreamImpl
    saOutputStreamImpl& operator=( const saOutputStreamImpl& );
    saOutputStreamImpl( const saOutputStreamImpl& );
  };

  saOutputStreamImpl::saOutputStreamImpl ( saOutputSink* sink )
    : outputSink( sink ),
      isUnderRun( outputSink->UnderRun() ), 
      framesPerSecond( outputSink->GetFramesPerSecond() ),
      samplesPerFrame( outputSink->GetSamplesPerFrame() ),
      framesPerFragment( outputSink->GetFramesPerFragment() ),
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      framesWrittenToStream( 0 ),
      framesWrittenToFragment( 0 ),
      framesSpaceRemainingInFragment( 0 ),
      currentFragment( 0 )
  {
  }

  saOutputStreamImpl::~saOutputStreamImpl( ) throw()
  { 
    // Flush current fragment
    if ((currentFragment != 0) && (framesWrittenToFragment > 0))
      {
        outputSink->Dispatch(currentFragment, framesWrittenToFragment);
      }
  }
  
  std::size_t saOutputStreamImpl::Write( std::vector<saSample>& buffer,
                                         const std::size_t framesToWrite )
  {
    // Initialise
    std::size_t samplesWrittenToFragment = 0;
    std::size_t samplesSpaceRemainingInFragment = 0;
    if (currentFragment != 0)
      {
        samplesSpaceRemainingInFragment = 
          samplesPerFrame*framesSpaceRemainingInFragment;
        samplesWrittenToFragment = samplesPerFrame*framesWrittenToFragment;
      }

    // Write. Copy samples from the buffer into a fragment Collect'ed from
    // the device. As the samples are copied they are are transformed to the
    // saSinkType. When the fragment is full, or there are no more samples,
    // Dispatch the fragment to the device.
    std::size_t samplesWritten = 0;
    std::size_t samplesLeftToWrite = samplesPerFrame*framesToWrite;
    while (samplesLeftToWrite > 0)
      {
        if (samplesSpaceRemainingInFragment >= samplesLeftToWrite)
          {
            std::transform(buffer.begin()+static_cast<long>(samplesWritten), 
                           buffer.begin()+
                           static_cast<long>(samplesWritten+samplesLeftToWrite), 
                           currentFragment+samplesWrittenToFragment,
                           saSampleToSink<saOutputSink::saSinkType>());
            samplesWrittenToFragment += samplesLeftToWrite;
            samplesWritten += samplesLeftToWrite;
            samplesSpaceRemainingInFragment -= samplesLeftToWrite;
            samplesLeftToWrite = 0; 
          }
        else if (samplesSpaceRemainingInFragment > 0)
          {
            std::transform(buffer.begin()+static_cast<long>(samplesWritten), 
                           buffer.begin()+static_cast<long>(samplesWritten)+
                           static_cast<long>(samplesSpaceRemainingInFragment), 
                           currentFragment+samplesWrittenToFragment,
                           saSampleToSink<saOutputSink::saSinkType>());
            samplesWrittenToFragment += samplesSpaceRemainingInFragment;
            samplesWritten += samplesSpaceRemainingInFragment;
            samplesLeftToWrite -= samplesSpaceRemainingInFragment; 
            samplesSpaceRemainingInFragment = 0;
          }
        else
          {
            if (currentFragment != 0)
              {
                outputSink->Dispatch(currentFragment, 
                                     samplesWrittenToFragment/samplesPerFrame);
              }
            // Get a memory fragment from the sink device.
            // The output samples will be copied into this fragment
            currentFragment = outputSink->Claim();
            samplesWrittenToFragment = 0;
            samplesSpaceRemainingInFragment = samplesPerFrame*framesPerFragment;
          }
      }

    // Done
    framesSpaceRemainingInFragment = 
      samplesSpaceRemainingInFragment/samplesPerFrame;
    framesWrittenToFragment = samplesWrittenToFragment/samplesPerFrame;
    std::size_t framesWritten = samplesWritten/samplesPerFrame;
    framesWrittenToStream += framesWritten;
    return framesWritten;
  }

  saOutputStream*
  saOutputStreamOpenDevice( const std::string& deviceName, 
                            const std::size_t framesPerSecond,
                            const std::size_t msDeviceLatency,
                            const std::size_t channels )
  {
    saOutputSink* deviceSink = 
      saOutputSinkOpenDevice
      (deviceName, framesPerSecond, msDeviceLatency, channels);
    
    return new saOutputStreamImpl( deviceSink );
  }
  
  saOutputStream*
  saOutputStreamOpenFile( const std::string& fileName,
                          const std::size_t framesPerSecond,
                          const std::size_t channels)
  {
    saOutputSink* fileSink = saOutputSinkOpenFile( fileName, 
                                                   framesPerSecond, 
                                                   channels );
    
    return new saOutputStreamImpl( fileSink );
  }
  
}
  
