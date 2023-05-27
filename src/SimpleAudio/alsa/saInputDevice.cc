/// \file saInputDevice.cc
/// \brief Implementation of reading from a simple audio input device for 
///  OSS on FreeBSD

// StdC++ headers
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <type_traits>

// StdC headers
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <stdint.h>
#include <alsa/asoundlib.h>

#include "saInputSource.h"

namespace SimpleAudio
{
  class saInputDevice : public saInputSource
  {
  public:
    saInputDevice( const std::string name, 
                   const std::size_t fps, 
                   const std::size_t ms,
                   const std::size_t channels,
                   const bool debug);

    ~saInputDevice() throw();

    // \brief Read a fragment from a simple audio input
    // \param numValid number of valid frames in the fragment
    // \return saInputFragment 
    saInputFragment Collect(std::size_t& numValid);

    /// \brief Release fragment storage
    /// \param fragment 
    void Release([[maybe_unused]] const saInputFragment fragment) { }

    /// \brief Clear the source. Assumes only one stream opens the device.
    void Clear( ) { isOverRun = false; }

    /// \brief Check for source over-runs
    /// \return device over run
    bool OverRun( ) { return isOverRun; }

    /// \brief Check a simple audio input device for end-of-data
    /// (Only makes sense for files).
    /// \return end of source found
    bool EndOfSource( ) { return false; }

    /// \brief Get device frame rate
    /// \return frames per second
    std::size_t GetFramesPerSecond( ) const { return framesPerSecond; }

    /// \brief Get samples per frame
    /// \return samples per frame
    std::size_t GetSamplesPerFrame( ) const { return samplesPerFrame; }
    
    /// \brief Get device frames per fragment
    /// \return frames per fragment
    std::size_t GetFramesPerFragment( ) const { return framesPerFragment; }
    
    /// \brief Get device frames per source
    /// \return device frames per source
    std::size_t GetFramesPerSource( ) const { return framesPerSource; }

    /// \brief Get device frames read from source
    /// \return device frames read from source
    std::size_t GetFramesReadFromSource() const { return framesReadFromDevice; }

    /// \brief Get device latency (buffer size) in milliseconds
    /// \return device latency
    std::size_t GetMsDeviceLatency( ) const { return msDeviceLatency; }

  private:
    /// Source device name
    std::string sourceName;
        
    /// Source over-run flag
    bool isOverRun;
        
    /// Frames per second
    std::size_t framesPerSecond;
    
    /// Milli-seconds of latency. Determines fragment size
    std::size_t msDeviceLatency;

    /// Device handle
    snd_pcm_t *handle;

    /// Number of bytes in a sample
    std::size_t bytesPerSample;

    /// Samples per frame
    std::size_t samplesPerFrame;
    
    /// Frames per fragment
    std::size_t framesPerFragment;

    /// Bytes per fragment
    std::size_t bytesPerFragment;

    /// Bytes per frame
    std::size_t bytesPerFrame;

    /// Samples per fragment
    std::size_t samplesPerFragment;

    /// Number of frames stored in the source
    std::size_t framesPerSource;

    /// Frames read from device 
    std::size_t framesReadFromDevice;

    /// Array of samples sub-divided into fragments
    std::vector<saSourceType> sourceBuffer;

    /// Use this to access source buffer 
    saInputFragment sourcePtr;

    /// For debugging
    bool debug;
  };

  saInputDevice::saInputDevice(const std::string name, 
                               const std::size_t fps,
                               const std::size_t ms,
                               const std::size_t channels,
                               const bool _debug)
    : sourceName( name ), 
      isOverRun( false ),
      framesPerSecond( fps ),
      msDeviceLatency( ms ),
      handle( 0 ),
      bytesPerSample( sizeof(saInputSource::saSourceType) ),
      samplesPerFrame( channels ),
      framesPerFragment( ms*fps/1000 ),
      bytesPerFragment( bytesPerSample*samplesPerFrame*framesPerFragment ),
      bytesPerFrame( bytesPerSample*samplesPerFrame ),
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      framesPerSource( framesPerFragment ),
      framesReadFromDevice( 0 ),
      sourceBuffer( samplesPerFragment, 0 ),
      sourcePtr( &(sourceBuffer[0]) ),
      debug( _debug )
  {
    // Open the input device
    int err = snd_pcm_open(&handle, 
                           sourceName.c_str(), 
                           SND_PCM_STREAM_CAPTURE, 
                           0);
    if (err < 0) 
      {
        throw std::runtime_error(std::string("can't open audio device: ") +
                                 std::string(snd_strerror(err)));
      }
                   
    snd_pcm_hw_params_t *hw_params;
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) 
      {
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error
          (std::string("can't alloc. hardware parameters structure: ") +
           std::string(snd_strerror(err)));
      }
                                 
    if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) 
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error
          (std::string("cannot initialize hardware parameter structure: ") +
           std::string(snd_strerror(err)));
      }
        
    if ((err = snd_pcm_hw_params_set_access 
         (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) 
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error(std::string("cannot set access type: ") +
                                 std::string(snd_strerror(err))); 
      }

    if ( (bytesPerSample != sizeof(int16_t)) ||
         std::is_unsigned<saSourceType>() )
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error("source sample inconsistent");
      }
    if ((err = snd_pcm_hw_params_set_format
         (handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) 
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error(std::string("cannot set sample format: ") +
                                 std::string(snd_strerror(err))); 
      }
        
    unsigned int snd_framesPerSecond = (unsigned int) framesPerSecond;
    if ((err = snd_pcm_hw_params_set_rate_near 
         (handle, hw_params, &snd_framesPerSecond, 0)) < 0) 
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error(std::string("cannot set sample rate: ") +
                                 std::string(snd_strerror(err))); 
      }
     
    unsigned int snd_samplesPerFrame = (unsigned int) samplesPerFrame;
    if ((err = snd_pcm_hw_params_set_channels 
         (handle, hw_params, snd_samplesPerFrame)) < 0) 
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error(std::string("cannot set channel count: ") +
                                 std::string(snd_strerror(err))); 
      }
        
    if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) 
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error(std::string("cannot set parameters: ") +
                                 std::string(snd_strerror(err))); 
      }
    snd_pcm_hw_params_free (hw_params);
    
    if ((err = snd_pcm_prepare (handle)) < 0) 
      {
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error
          (std::string("cannot prepare audio interface for use: ") +
           std::string(snd_strerror(err)));
      }
  }

  saInputDevice::~saInputDevice( ) throw()
  { 
    try
      {
        // Close device
        snd_pcm_close(handle);
        snd_config_update_free_global();
      }
    catch(std::exception& excpt)
      {
        std::cerr << excpt.what() << std::endl ; 
      }
  }

  static std::size_t saInputDeviceRead(snd_pcm_t* handle,
                                       saInputSource::saSourceType* buffer, 
                                       const std::size_t framesToRead,
                                       bool& isOverRun)
  {
    long numRead = snd_pcm_readi(handle, buffer, framesToRead);

    int err = (int)numRead;
    if (err == -EPIPE) 
      {
        numRead = 0;
        isOverRun = true;
      }
    else if (err == -EBADFD) 
      {
        numRead = 0;
        throw std::runtime_error
          ( std::string("audio input device read failed: ") + 
            std::string("PCM is not in the right state") );
      }
    else if (err == -ESTRPIPE) 
      {
        numRead = 0;
        throw std::runtime_error
          ( std::string("audio input device read failed: ") + 
            std::string("a suspend event occurred") );
      }
    else if (err < 0)
      {
        numRead = 0;
        throw std::runtime_error
          ( std::string("audio input device read failed: ") + 
            std::string(snd_strerror(err))); 
      }

    return static_cast<std::size_t>(numRead);
  }

  saInputSource::saInputFragment saInputDevice::Collect(std::size_t& numValid)
  {
    // Read from device into this fragment
    std::size_t framesRead = 
      saInputDeviceRead(handle, sourcePtr, framesPerFragment, isOverRun);
    
    // Done
    numValid = framesRead;
    framesReadFromDevice += numValid;
    return sourcePtr;
  }

  saInputSource* saInputSourceOpenDevice(const std::string& deviceName, 
                                         const std::size_t framesPerSecond,
                                         const std::size_t msDeviceLatency,
                                         const std::size_t channels,
                                         const bool debug)
  {
    return new saInputDevice
      (deviceName, framesPerSecond, msDeviceLatency, channels, debug);
  }

}
