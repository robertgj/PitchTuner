/// \file PreProcessor.h
/// \brief Pre-processing for the pitch estimator
///
/// Read and preprocess the input signal:
///   - low-pass filter and decimate the input signal
///   - high-pass filter to remove DC component
///   - apply AGC 

#include <vector>
#include <deque>
#include <string>

#include "saSample.h"

#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

using namespace SimpleAudio;

/// \class PreProcessorImpl
/// \brief Implementation of preprocessing of the input signal
/// Low-pass filter the input audio waveform and then as required, decimate,
/// high-pass filter and apply AGC. The preprocessed waveform is stored in a
/// double ended queue.
class PreProcessor
{
public:
  /// Constructor for PreProcessor
  PreProcessor() {};

  /// Destructor for PreProcessor
  virtual ~PreProcessor() throw() { };

  /// Sub-sampled sample rate
  /// \return sub-sampled sample rate
  virtual float SubSampleRate() = 0;

  /// Sub-sampled window length in samples
  /// \return sub-sampled window length in samples
  virtual std::size_t WindowLength() = 0;

  /// Sub-sampled max. correlation length in samples
  /// \return sub-sampled window length in samples
  virtual std::size_t MaxLags() = 0;

  /// Read a vector of preprocessed input values
  /// \return samples 
  virtual const std::deque<saSample>& Read( ) = 0;

protected:
  /// Disallow assignment to PreProcessor
  PreProcessor& operator=( const PreProcessor& );

  /// Disallow copy constructor of PreProcessor
  PreProcessor( const PreProcessor& );
};

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
                                  bool debug );

#endif
