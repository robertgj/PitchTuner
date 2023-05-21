/// \file PreProcessor.cc

// StdC++ headerse
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <iterator>

// StdC headers
#include <cmath>
#include <ctime>
#include <cassert>

// Local include files
#include "saSample.h"
#include "saInputStream.h"
#include "PreProcessor.h"
#include "ButterworthFilter.h"
#include "AutomaticGainControl.h"

using namespace SimpleAudio;

/// \struct DecimationFilter
/// Functor that filters then decimates the input signal
/// \tparam T typename of the input samples
template <typename T>
struct DecimationFilter
{
  DecimationFilter(std::vector<T>& _u, Filter<T>& _f, std::size_t _dec) 
    : u(_u), f(_f), dec(_dec), base(0) { }

  /// Filter and then decimate the input signal
  /// \return the filter output value
  T operator() ()
  { 
    T val = 0; 
    for(std::size_t i=0; i<dec; i++) 
      { 
        val = f(u[base++]); 
      } 
    return val;
  }

private:
  std::vector<T>& u;
  Filter<T>& f;
  std::size_t dec;
  std::size_t base;
};

class PreProcessorImpl : public PreProcessor
{
public:
  /// Constructor for PreProcessorImpl
  ///
  //  Constructor for PreProcessorImpl. 
  //  Note that this object doesn't take
  //  ownership of the inputStream
  //
  //  \param \e inputStream input stream
  //  \param \e channel channel from input frame
  //  \param \e msWindow window size in ms
  //  \param \e msTmax correlation width in ms
  //  \param \e msTsample pitch sample interval in ms
  //  \param \e subSample sub-sampling ratio
  //  \param \e subSampleLpCutoff low-pass filter for sub-sampling
  //  \param \e baseLineHpCutoff high-pass filter for removing 50 and 60Hz
  //  \param \e disableHpFilter disable the high-pass filter
  //  \param \e disableAgc disable Automatic Gain Control
  //  \param \e enable debug messages
  //  \return Nil
  PreProcessorImpl( saInputStream* inputStream, 
                    std::size_t channel, 
                    std::size_t msWindow, 
                    std::size_t msTmax, 
                    std::size_t msTsample,
                    std::size_t subSample,
                    float subSampleLpCutoff,
                    float baseLineHpCutoff,
                    bool disableHpFilter,
                    bool disableAgc,
                    bool debug );
  
  /// Destructor for PreProcessor
  ///
  //  Destructor for PreProcessor
  ~PreProcessorImpl() throw();

  /// Sub-sampled sample rate
  ///
  //  Get sub-sampled sample rate
  //
  // \return sub-sampled sample rate
  float SubSampleRate() 
  { 
    return static_cast<float>(sampleRate)/(float)subSample; 
  }

  /// Sub-sampled window length in samples
  ///
  //  Sub-sampled window length in samples
  //
  // \return sub-sampled window length in samples
  std::size_t WindowLength() { return subSampledWindow; }

  /// Sub-sampled max. correlation length in samples
  ///
  //  Sub-sampled max. correlation length in samples
  //
  // \return Sub-sampled max. correlation length in samples
  std::size_t MaxLags() { return subSampledTmax; }

  /// Read a deque of preprocessed input values
  ///
  //  Read a deque of preprocessed samples. 
  //  Reader can't modify the returned deque.
  //
  //  \return \c std::deque of preprocessed samples
  const std::deque<saSample>& Read( );

private:
  /// Disallow assignment to PreProcessorImpl
  PreProcessorImpl& operator=( const PreProcessorImpl& );
  
  /// Disallow copy constructor of PreProcessorImpl
  PreProcessorImpl( const PreProcessorImpl& );

  // Input stream
  saInputStream* inputStream;

  /// Channel from frame
  std::size_t channel;
  
  /// Input sub-sampling ratio
  std::size_t subSample;

  // Debug
  bool debug;
  bool disableHpFilter;
  bool disableAgc;

  /// Sample rate in Hz
  std::size_t sampleRate;

  /// Number of samples in a frame
  std::size_t samplesPerFrame;

  /// Pitch sample interval in samples after sub-sampling
  std::size_t subSampledInterval;

  /// Interval between pitch updates in samples before sub-sampling
  //  Must be a multiple of subSample
  std::size_t sampleInterval;

  /// Window size in samples after sub-sampling
  std::size_t subSampledWindow; 

  /// Correlation width in samples after sub-sampling
  std::size_t subSampledTmax; 

  /// Length of output sequence supplied by Read()
  std::size_t subSampledOutputSize; 

  /// Low-pass filter
  ButterworthLowPass4thOrderFilter<saSample> lpFilter;

  /// High-pass baseline filter
  ButterworthHighPass3rdOrderFilter<saSample> hpFilter;

  /// AGC
  AutomaticGainControl<saSample> agc;

  /// Buffer for samples from stream
  std::vector<saSample> y;

  // Subsequent storage
  std::vector<saSample> ylp;
  std::vector<saSample> yhp;
  std::vector<saSample> yagc;
  std::deque<saSample> outputDeque;

  /// Storage for debugging
  std::vector<saSample> yList;
  std::vector<saSample> ylpList;
  std::vector<saSample> yhpList;
  std::vector<saSample> yagcList;
};

PreProcessorImpl::PreProcessorImpl( saInputStream* _inputStream,
                                    std::size_t _channel, 
                                    std::size_t _msWindow, 
                                    std::size_t _msTmax, 
                                    std::size_t _msTsample,
                                    std::size_t _subSample,
                                    float _subSampleLpCutoff,
                                    float _baseLineHpCutoff,
                                    bool _disableHpFilter,
                                    bool _disableAgc,
                                    bool _debug = false)
  : inputStream( _inputStream ),
    channel( _channel ),
    subSample( _subSample ),
    debug( _debug ),
    disableHpFilter( _disableHpFilter ),
    disableAgc( _disableAgc ),
    sampleRate( inputStream->GetFramesPerSecond() ),
    samplesPerFrame( inputStream->GetSamplesPerFrame() ),
    subSampledInterval( (_msTsample*sampleRate)/(1000*subSample) ),
    sampleInterval( subSample*subSampledInterval ),
    subSampledWindow( (_msWindow*sampleRate)/(1000*subSample) ),
    subSampledTmax( (_msTmax*sampleRate)/(1000*subSample) ),
    subSampledOutputSize( subSampledWindow+subSampledTmax+1 ),
    lpFilter( _subSampleLpCutoff, sampleRate ),
    hpFilter( _baseLineHpCutoff, sampleRate/subSample ),
    agc( sampleRate/subSample )
{
  // Reserve space in vectors
  y.reserve( sampleInterval*samplesPerFrame );
  ylp.reserve( sampleInterval );
  yhp.reserve( subSampledInterval );
  yagc.reserve( subSampledInterval );
}

PreProcessorImpl::~PreProcessorImpl() throw()
{
  if (debug)
    {
      {
        std::ofstream of("y.txt");
        copy(yList.begin(), yList.end(), 
             std::ostream_iterator<saSample>(of, "\n"));
      }
      {
        std::ofstream of("ylp.txt");
        copy(ylpList.begin(), ylpList.end(), 
             std::ostream_iterator<saSample>(of, "\n"));
      }
      {
        std::ofstream of("yhp.txt");
        copy(yhpList.begin(), yhpList.end(), 
             std::ostream_iterator<saSample>(of, "\n"));
      }
      {
        std::ofstream of("yagc.txt");
        copy(yagcList.begin(), yagcList.end(), 
             std::ostream_iterator<saSample>(of, "\n"));
      }
    }
}

const std::deque<saSample>& PreProcessorImpl::Read( )
{
  // Check for over-run
  if ( inputStream->OverRun() )
    {
      // Clear
      inputStream->Clear();
      outputDeque.assign(subSampledOutputSize, 0);
      if (debug)
        {
          std::cerr << "Stream reset after overrun\n";
        }
    }

  // Discard oldest elements
  if (outputDeque.size() >= subSampledInterval)
    {
      outputDeque.erase(outputDeque.begin(), 
                        outputDeque.begin()+
                        static_cast<long>(subSampledInterval));
    }
  else if ( outputDeque.size() > 0)
    {
      // Should not get here!
      outputDeque.resize(0);
    }

  // Loop filling the deque
  while (outputDeque.size() < subSampledOutputSize)
    {
      // Read from stream
      y.resize(0);
      std::size_t numRead = inputStream->Read( y, sampleInterval, channel );
      if (numRead != sampleInterval)
        {
          // Sanity check
          if ( !inputStream->EndOfSource() )
            {
              inputStream->Clear();
              outputDeque.assign(subSampledOutputSize, 0);
              if (debug)
                {
                  std::cerr << "Stream reset after under-run\n";
                }
              return outputDeque;
            }
          // Pad outputDeque to required size
          y.insert(y.end(), sampleInterval-y.size(), 0);
        }

      // Do low-pass filter
      ylp.resize(0);
      if (subSample > 1)
        {
          std::generate_n(std::back_inserter(ylp), subSampledInterval, 
                          DecimationFilter<saSample>(y, lpFilter, subSample));
        }
      else
        {
          std::transform(y.begin(), y.end(), std::back_inserter(ylp), 
                         updateFilter<saSample>(&lpFilter));
        }

      if ( disableHpFilter && disableAgc )
        {
          // Copy to deque
          outputDeque.insert(outputDeque.end(), ylp.begin(), ylp.end());
        }
      else if ( (!disableHpFilter) && disableAgc )
        {
          // Do high-pass filter
          yhp.resize(0);
          std::transform(ylp.begin(), ylp.end(), std::back_inserter(yhp), 
                         updateFilter<saSample>(&hpFilter));

          // Copy to deque
          outputDeque.insert(outputDeque.end(), yhp.begin(), yhp.end());
        }
      else if ( disableHpFilter && (!disableAgc) )
        {
          // Do AGC
          yagc.resize(0);
          std::transform(ylp.begin(), ylp.end(), std::back_inserter(yagc),
                         updateFilter<saSample>(&agc));
          
          // Copy to deque
          outputDeque.insert(outputDeque.end(), yagc.begin(), yagc.end());
        }
      else
        {
          // Do high-pass filter
          yhp.resize(0);
          std::transform(ylp.begin(), ylp.end(), std::back_inserter(yhp), 
                         updateFilter<saSample>(&hpFilter));

          // Do AGC
          yagc.resize(0);
          std::transform(yhp.begin(), yhp.end(), std::back_inserter(yagc),
                         updateFilter<saSample>(&agc));
          
          // Copy to deque
          outputDeque.insert(outputDeque.end(), yagc.begin(), yagc.end());
        }
               
      // Save
      if ( debug )
        {
          yList.insert(yList.end(), y.begin(), y.end());
          ylpList.insert( ylpList.end(), ylp.begin(), ylp.end() );
          yhpList.insert( yhpList.end(), yhp.begin(), yhp.end() );
          yagcList.insert( yagcList.end(), yagc.begin(), yagc.end() );
        }
    }

  return outputDeque;
}

PreProcessor* PreProcessorCreate( saInputStream* inputStream, 
                                  std::size_t channel, 
                                  std::size_t msWindow, 
                                  std::size_t msTmax, 
                                  std::size_t msTsample,
                                  std::size_t subSample,
                                  float subSampleLpCutoff,
                                  float baseLineHpCutoff,
                                  bool disableHpFilter,
                                  bool disableAgc,
                                  bool debug)
{
  return new PreProcessorImpl( inputStream, channel,
                               msWindow, msTmax, msTsample, 
                               subSample, subSampleLpCutoff, baseLineHpCutoff, 
                               disableHpFilter, disableAgc, debug );
}
