/// \file PitchTracker.h
/// \brief Public interface for a pitch estimator

#include <deque>

#include "saSample.h"

#ifndef __PITCHTRACKER_H__
#define __PITCHTRACKER_H__

using namespace SimpleAudio;

class PitchTracker
{
public:
  /// Constructor for PitchTracker
  PitchTracker() { };
  
  /// Destructor for PitchTracker
  virtual ~PitchTracker() throw() { };

  /// Read a single pitch value from the pitch tracker
  ///
  ///  \return \c float
  virtual float EstimatePitch( const std::deque<saSample>& ) = 0;

  /// Read the voiced flag from the pitch tracker
  ///
  /// True if pitch estimate is valid
  ///
  ///  \return \c bool
  virtual bool Voiced() = 0;

protected:
  /// Disallow assignment to PitchTracker
  PitchTracker& operator=( const PitchTracker& );

  /// Disallow copy constructor of PitchTracker
  PitchTracker( const PitchTracker& );
};

/// \brief Create a PitchTracker object
/// \param sampleRate the pre-processed audio waveform sample rate
/// \param WindowLength the time window on the waveform over which the
///        algorithm is applied
/// \param MaxLags the maximum possible pitch interval considered (adds to the
///                    window length latency)
/// \param threshold the upper threshold for a minimum of the cumulative mean
///        distance function to be considered a pitch estimate
/// \param removeDC apply the DC removal algorithm
/// \param debug store internal values for debugging
/// \return PitchTracker object
PitchTracker* PitchTrackerCreate( float sampleRate, 
                                  std::size_t WindowLength,
                                  std::size_t MaxLags, 
                                  float threshold,
                                  bool removeDC,
                                  bool debug );
#endif
