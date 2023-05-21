/// \file saInputStream.cc
/// \brief Implementation of a simple audio input stream

#include <string>
#include <vector>
#include <list>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>

#include <cassert>

#include "saSample.h"
#include "saInputStream.h"
#include "saInputSource.h"

namespace SimpleAudio
{

  // Useful functor
  template<typename T>
  struct DecimateSource
  {
    DecimateSource(T* _base, std::size_t _dec) : base(_base), dec(_dec) { }
    saSample operator() ()
    { 
      saSample val = saSampleFromSource<T>()(*base);
      base += dec;
      return val;
    }
  private:
    T* base;
    std::size_t dec;
  };

  class saInputStreamImpl : public saInputStream
  {
  public:
    /// \brief Create a simple input audio stream
    ///
    /// Creates and initialises an \c saInputStreamImpl implementation of
    /// saInputStreamBase.
    /// \param source handle reference to the audio input
    /// \param depth size of input stream buffer
    saInputStreamImpl( saInputSource* source, std::size_t depth);

    /// \brief Create a simple audio stream
    ///
    /// Creates and initialises an \c saInputStreamImpl implementation of
    /// \c saInputStreamBase. Allows specification of the deque size
    /// \param source audio input source
    saInputStreamImpl( saInputSource* source );

    ~saInputStreamImpl( ) throw();

    /// \brief Read a buffer of frames from the saInputStream
    /// \param buffer vector of saSample
    /// \param framesToRead frames to read from the source
    /// \return number of frames read
    std::size_t Read( std::vector< saSample >& buffer,
                      const std::size_t framesToRead);

    /// \brief Read a buffer of samples from the saInputStream
    ///
    /// Read a buffer of samples from the saInputStream
    /// The sample is selected from each frame 
    /// \param buffer vector of saSample
    /// \param framesToRead frames to read from the stream
    /// \param sampleInFrame sample in frame to read
    /// \return number of frames read
    std::size_t Read( std::vector< saSample >& buffer,
                      const std::size_t framesToRead,
                      const std::size_t sampleInFrame);

    /// \brief Clear the stream
    void Clear( );

    /// \brief Check for stream over-runs
    /// \return stream over run occurred
    bool OverRun( );

    /// \brief Check for stream end-of-source
    /// \return stream end of source occurred
    bool EndOfSource( );

    /// \brief Get frame rate
    /// \return frames per second
    std::size_t GetFramesPerSecond() const { return framesPerSecond; }

    /// \brief Get samples per frame
    /// \return samples per frame
    std::size_t GetSamplesPerFrame() const { return samplesPerFrame; }

    /// \brief Get frames per fragment
    /// \return frames per fragment
    std::size_t GetFramesPerFragment() const { return framesPerFragment; }

    /// \brief Get capacity of the stream
    /// \return frames per stream
    std::size_t GetFramesPerStream() const { return framesPerStream; }

    /// \brief Get frames read from the stream
    /// \return frames read from the stream
    std::size_t GetFramesReadFromStream() const { return framesReadFromStream; }

  private:

    /// Read source
    std::unique_ptr<saInputSource> inputSource;

    /// Flag source over-run
    bool isOverRun;

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

    /// Frames read from stream
    std::size_t framesReadFromStream;

    /// Frames in this fragment
    std::size_t framesThisFragment;

    /// Frames read from fragment
    std::size_t framesReadFromFragment;

    /// Frames remaining in fragment
    std::size_t framesRemainingInFragment;

    /// Current fragment
    saInputSource::saSourceType* currentFragment;

    /// Disallow assignment to saInputStreamImpl
    saInputStreamImpl& operator=( const saInputStreamImpl& );

    /// Disallow copy constructor of saInputStreamImpl
    saInputStreamImpl( const saInputStreamImpl& );
  };

  saInputStreamImpl::saInputStreamImpl ( saInputSource* source )
    : inputSource( source ), 
      isOverRun( false ), 
      framesPerSecond( inputSource->GetFramesPerSecond() ),
      samplesPerFrame( inputSource->GetSamplesPerFrame() ),
      framesPerFragment( inputSource->GetFramesPerFragment() ),
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      framesPerStream( inputSource->GetFramesPerSource() ),
      framesReadFromStream( 0 ),
      framesThisFragment( 0 ),
      framesReadFromFragment( 0 ),
      framesRemainingInFragment( 0 ),
      currentFragment( 0 )
  {
  }

  saInputStreamImpl::~saInputStreamImpl( ) throw()
  { 
  }
  
  std::size_t saInputStreamImpl::Read( std::vector< saSample >& buffer,
                                       const std::size_t framesToRead )
  {
    // Data available? Try to fulfill a request
    std::size_t samplesRemainingInFragment = 0; 
    std::size_t samplesReadFromFragment = 0;
    saInputSource::saSourceType* nextSourceInput = 0;
    if (currentFragment != 0)
      {
        samplesRemainingInFragment = samplesPerFrame*framesRemainingInFragment;
        samplesReadFromFragment = samplesPerFrame*framesReadFromFragment;
        nextSourceInput = &(currentFragment[samplesReadFromFragment]);
      }

    // Read. Collect a fragment from the device. Copy samples from the fragment
    // into the buffer. As the samples are copied they are are transformed
    // from saSourceType to saSample. Release the fragment and Collect another.
    std::size_t samplesRead = 0;
    std::size_t samplesToRead = samplesPerFrame*framesToRead;
    while (samplesToRead > 0)
      {
        if (samplesRemainingInFragment >= samplesToRead)
          {
            std::transform(nextSourceInput, 
                           nextSourceInput+samplesToRead, 
                           std::back_inserter(buffer),
                           saSampleFromSource<saInputSource::saSourceType>());
            nextSourceInput += samplesToRead;
            samplesReadFromFragment += samplesToRead;
            samplesRead += samplesToRead;
            samplesRemainingInFragment -= samplesToRead;
            samplesToRead = 0; 
          }
        else if (samplesRemainingInFragment > 0)
          {
            std::transform(nextSourceInput, 
                           nextSourceInput+samplesRemainingInFragment,
                           std::back_inserter(buffer),
                           saSampleFromSource<saInputSource::saSourceType>());
            nextSourceInput += samplesRemainingInFragment;
            samplesReadFromFragment += samplesRemainingInFragment;
            samplesRead += samplesRemainingInFragment;
            samplesToRead -= samplesRemainingInFragment; 
            samplesRemainingInFragment = 0;
          }
        else if ( EndOfSource() )
          {
            break;
          }
        else
          {
            if (currentFragment != 0)
              {
                inputSource->Release(currentFragment);
              }
            currentFragment = inputSource->Collect(framesThisFragment);
            nextSourceInput = currentFragment;
            samplesReadFromFragment = 0;
            framesRemainingInFragment = framesThisFragment;
            samplesRemainingInFragment = 
              samplesPerFrame*framesRemainingInFragment;
          }

        // Sanity checks
        assert( (samplesRemainingInFragment+samplesReadFromFragment) 
                == samplesPerFrame*framesThisFragment );
        assert( static_cast<std::size_t>(nextSourceInput-currentFragment)
                == samplesReadFromFragment );
      }
    
    // Done
    framesRemainingInFragment = samplesRemainingInFragment/samplesPerFrame;
    framesReadFromFragment = samplesReadFromFragment/samplesPerFrame;
    std::size_t framesRead = samplesRead/samplesPerFrame;
    framesReadFromStream += framesRead;
    return framesRead;
  }

  std::size_t saInputStreamImpl::Read( std::vector< saSample >& buffer,
                                       std::size_t framesToRead,
                                       const std::size_t sampleInFrame )
  {
    // Sanity check (sampleInFrame is zero-based)
    if (sampleInFrame >= samplesPerFrame)
      {
        throw std::runtime_error("Illegal sampleInFrame");
      }

    // Ignore sampleInFrame argument for efficiency
    if (samplesPerFrame == 1)
      {
        return Read( buffer, framesToRead );
      }

    // Data available? Try to fulfill a request
    saInputSource::saSourceType* nextSourceInput = 0;
    if (currentFragment != 0)
      {
        nextSourceInput = 
          &(currentFragment[samplesPerFrame*framesReadFromFragment]);
      }

    std::size_t framesRead = 0;
    std::size_t framesLeftToRead = framesToRead;
    while (framesLeftToRead>0)
      {
        if (framesRemainingInFragment > framesLeftToRead)
          {
            std::generate_n(std::back_inserter(buffer),
                            framesLeftToRead, 
                            DecimateSource<saInputSource::saSourceType>
                            (nextSourceInput+sampleInFrame, samplesPerFrame));

            nextSourceInput += samplesPerFrame*framesLeftToRead;
            framesReadFromFragment += framesLeftToRead;
            framesRead += framesLeftToRead;
            framesRemainingInFragment -= framesLeftToRead;
            framesLeftToRead = 0; 
          }
        else if (framesRemainingInFragment > 0)
          {
            std::generate_n(std::back_inserter(buffer),
                            framesRemainingInFragment, 
                            DecimateSource<saInputSource::saSourceType>
                            (nextSourceInput+sampleInFrame, samplesPerFrame));

            nextSourceInput += samplesPerFrame*framesRemainingInFragment;
            framesReadFromFragment += framesRemainingInFragment;
            framesRead += framesRemainingInFragment;
            framesLeftToRead -= framesRemainingInFragment; 
            framesRemainingInFragment = 0;
          }
        else if ( EndOfSource() )
          {
            break;
          }
        else
          {
            framesReadFromFragment = 0;
            if ( currentFragment != 0 )
              {
                inputSource->Release(currentFragment);
              }
            currentFragment = inputSource->Collect(framesThisFragment);
            framesRemainingInFragment = framesThisFragment;
            nextSourceInput = currentFragment;
          }

        // Sanity checks
        assert( (framesRemainingInFragment+framesReadFromFragment) 
                == framesThisFragment );
        assert( static_cast<std::size_t>(nextSourceInput-currentFragment)
                == samplesPerFrame*framesReadFromFragment );
      }

    // Done
    framesReadFromStream += framesRead;
    return framesRead;
  }

  inline bool saInputStreamImpl::OverRun( ) 
  { 
    return isOverRun || inputSource->OverRun(); 
  }

  inline bool saInputStreamImpl::EndOfSource( ) 
  { 
    return ((inputSource->EndOfSource() == true) && 
            (framesRemainingInFragment == 0));
  }

  inline void saInputStreamImpl::Clear( )
  {
    // Abandon this fragment
    framesRemainingInFragment = 0;
    currentFragment = 0;

    // Clear the input source
    inputSource->Clear();
    
    // Clear the flag
    isOverRun = false;
  }

  saInputStream*
  saInputStreamOpenDevice( const std::string& deviceName, 
                           const std::size_t framesPerSecond,
                           const std::size_t msDeviceLatency,
                           const std::size_t channels,
                           const bool debug )
  {
    saInputSource* deviceSource = 
      saInputSourceOpenDevice(deviceName, framesPerSecond, msDeviceLatency, 
                              channels, debug);

    return new saInputStreamImpl( deviceSource );
  }

  saInputStream*
  saInputStreamOpenFile( const std::string& fileName )
  {
    saInputSource* fileSource = saInputSourceOpenFile(fileName);

    return new saInputStreamImpl( fileSource );
  }

}
