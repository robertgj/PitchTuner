/// \file saOutputDevice.cc
/// \brief A simple audio output device for ALSA

// StdC++ headers
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <type_traits>

// StdC headers
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#include "saOutputSink.h"

namespace SimpleAudio
{
    class saOutputDevice : public saOutputSink
  {
  public:
    saOutputDevice( const std::string name, 
                    const std::size_t fps, 
                    const std::size_t ms, 
                    const std::size_t channels );

    ~saOutputDevice() throw();

    /// \brief Allocate a fragment for a simple audio output
    /// \return saOutputFragment 
    saOutputFragment Claim();

    /// \brief Dispatch a fragment to the output device
    /// \param fragment 
    /// \param numValid number of valid frames in the fragment 
    void Dispatch( saOutputFragment fragment, 
                   const std::size_t numValid);

    /// \brief Clear the sink. Assumes only one stream opens the device
    void Clear( ) { isUnderRun = false; }

    /// \brief Check for source over-runs
    /// \return under run occurred
    bool UnderRun( ) { return isUnderRun; }

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

    /// \brief Get device latency (buffer size) in milliseconds
    /// \return device latency
    std::size_t GetMsDeviceLatency() const { return msDeviceLatency; }

  private:
    /// Sink device name
    std::string sinkName;
        
    /// Source under-run flag
    bool isUnderRun;
        
    /// Frames per second
    std::size_t framesPerSecond;
    
    /// Milli-seconds of device buffer latency. Determines fragment size
    std::size_t msDeviceLatency;

    /// Device handle
    snd_pcm_t *handle;

    /// Samples per frame
    std::size_t samplesPerFrame;
    
    /// Frames per fragment
    std::size_t framesPerFragment;

    /// Samples per fragment
    std::size_t samplesPerFragment;

    /// Frames written to sink
    std::size_t framesWrittenToSink;
    
    /// Array of samples 
    std::vector<saOutputSink::saSinkType> sinkBuffer;
  };

  saOutputDevice::saOutputDevice(const std::string deviceName, 
                                 const std::size_t frameRate,
                                 const std::size_t ms,
                                 const std::size_t channels)
    : sinkName( deviceName ), 
      isUnderRun( false ),
      framesPerSecond( frameRate ),
      msDeviceLatency( ms ),
      handle( 0 ),
      samplesPerFrame( channels ),
      framesPerFragment( ms*frameRate/1000 ),
      samplesPerFragment( samplesPerFrame*framesPerFragment ),
      framesWrittenToSink( 0 ),
      sinkBuffer( samplesPerFragment, 0 )
  {
    // Open the output device
    int err = snd_pcm_open(&handle, 
                           sinkName.c_str(), 
                           SND_PCM_STREAM_PLAYBACK, 
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
                 
    if ( (sizeof(saSinkType) != sizeof(int16_t)) ||
         std::is_unsigned<saSinkType>() )
      {
        snd_pcm_hw_params_free (hw_params);
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error("sink sample inconsistent");
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
    
    if ((err = snd_pcm_nonblock (handle, 0)) < 0)
      {
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error
          (std::string("cannot set audio interface to blocking : ") +
           std::string(snd_strerror(err)));
      }
    
    if ((err = snd_pcm_prepare (handle)) < 0) 
      {
        snd_pcm_close(handle);
        snd_config_update_free_global();
        throw std::runtime_error
          (std::string("cannot prepare audio interface for use: ") +
           std::string(snd_strerror(err)));
      }
  }

  saOutputDevice::~saOutputDevice( ) throw()
  { 
    try
      {
        // Close audio device
        
        int err = snd_pcm_drain(handle);
        if (err == -ESTRPIPE)
          {
            throw std::runtime_error
              (std::string("audio output device write failed: ") + 
               std::string("a suspend event occurred") );
          }
        else if (err < 0)
          {
            throw std::runtime_error
              (std::string("audio output device write failed"));
          }
              
        snd_pcm_close(handle);
        snd_config_update_free_global();
      }
    catch(std::exception& excpt)
      {
        std::cerr << excpt.what() << std::endl ; 
      }
  }

  saOutputSink::saOutputFragment saOutputDevice::Claim()
    {
      return &(sinkBuffer[0]);
    }
  
  static void saOutputDeviceWrite(snd_pcm_t *handle, 
                                  saOutputSink::saSinkType* buffer, 
                                  const std::size_t framesToWrite,
                                  bool &isUnderRun)
  {
    long numWritten = snd_pcm_writei(handle, buffer, framesToWrite);

    int err = (int)numWritten;
    if (err == -EPIPE) 
      {
        numWritten = 0;
        isUnderRun = true;
      }
    else if (err == -EBADFD) 
      {
        numWritten = 0;
        throw std::runtime_error
          ( std::string("audio output device write failed: ") + 
            std::string("PCM is not in the right state") );
      }
    else if (err == -ESTRPIPE) 
      {
        numWritten = 0;
        throw std::runtime_error
          ( std::string("audio output device write failed: ") + 
            std::string("a suspend event occurred") );
      }
    else if (err < 0)
      {
        numWritten = 0;
        throw std::runtime_error
          ( std::string("audio output device write failed: ") + 
            std::string(snd_strerror(err))); 
      }
    else if (static_cast<std::size_t>(numWritten) != framesToWrite) 
      {
        throw std::runtime_error
          ( "audio output device write incomplete!" );
      }
  }

  void saOutputDevice::Dispatch(const saOutputSink::saOutputFragment fragment,
                                std::size_t framesToWrite)
  {
    // Write from fragment into device
    saOutputDeviceWrite( handle, fragment, framesToWrite, isUnderRun );
    framesWrittenToSink += framesToWrite;
  }

  saOutputSink* saOutputSinkOpenDevice( const std::string& deviceName, 
                                        const std::size_t framesPerSecond,
                                        const std::size_t msDeviceLatency,
                                        const std::size_t channels)
  {
    return new saOutputDevice
      (deviceName, framesPerSecond, msDeviceLatency, channels);
  }

}

