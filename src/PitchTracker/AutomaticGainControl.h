/// \file AutomaticGainControl.h
/// \brief Automatic gain control functor
///
///  This file implements a simple automatic gain control. The gain is
///  adjusted so that the peaks of the output signal lie between given
///  upper and lower thresholds. 

// StdC++ headers
#include <iostream>

// StdC headers
#include <cmath>

// Local headers
#include "Filter.h"

using namespace SimpleAudio;

#ifndef __AUTOMATICGAINCONTROL_H__
#define __AUTOMATICGAINCONTROL_H__

/// \class AutomaticGainControl
/// \brief Implementation of an automatic gain control functor
/// \tparam T the type of the internal states
template <typename T>
class AutomaticGainControl : public Filter<T>
{
public:
  /// \brief Constructor for an automatic gain control functor
  /// \param _sampleRate input signal sample rate
  /// \param _fastTauMs time constant in ms for reducing gain
  /// \param _slowTauMs time constant in ms for increasing gain
  /// \param _peakTauMs time constant in ms for reducing gain on peaks
  /// \param _lowerPeakThreshold lower threshold on peak
  /// \param _upperPeakThreshold upper threshold on peak
  /// \param _gainMin minimum gain
  /// \param _gainMax maximum gain
  /// \param _debug
  AutomaticGainControl (const std::size_t _sampleRate,
                        const std::size_t _fastTauMs = 50,
                        const std::size_t _slowTauMs = 50,
                        const std::size_t _peakTauMs = 50,
                        const float _lowerPeakThreshold = 0.4f,
                        const float _upperPeakThreshold = 0.6f,
                        const float _gainMin = 0.1f,
                        const float _gainMax = 10.0f,
                        const bool _debug = false) : 
    lowerPeakThreshold(_lowerPeakThreshold),
    upperPeakThreshold(_upperPeakThreshold),
    gainMin(_gainMin), 
    gainMax(_gainMax), 
    // Convert time constants to a K factor by n=tau*sampleRate, K^n = e^(-1)
    fastK( expf(-1000.0f/(float)(_fastTauMs*_sampleRate)) ),
    slowK( expf(-1000.0f/(float)(_slowTauMs*_sampleRate)) ),
    peakK( expf(-1000.0f/(float)(_peakTauMs*_sampleRate)) ),
    gain(1), 
    peak(0),
    debug(_debug)
  {  
  }

  ~AutomaticGainControl () throw() {}

  /// Functor that implements an automatic gain control
  /// \param u the input
  /// \return the output
  T operator()(T& u)
  {
    // Update peak
    T U = gain*u;
    if ((U < 0) && (-U > peak))
      {
        peak = -U;
      }
    else if ((U > 0) && (U > peak))
      {
        peak = U;
      }
    peak *= peakK;

    // Update gain
    if ((peak < lowerPeakThreshold) && (gain < gainMax))
      {
        gain /= slowK;
      }
    else if ((peak > upperPeakThreshold) && (gain > gainMin))
      {
        gain *= fastK;
      }

    if (debug)
      {
        std::cout << u << " " << U << " " << gain << " " << peak << std::endl;
      }

    return U;
  }

private:

  // AGC parameters
  T lowerPeakThreshold; ///< Lower threshold of peak value under gain control
  T upperPeakThreshold; ///< Upper threshold of peak value under gain control
  T gainMin;            ///< Minimum gain
  T gainMax;            ///< Maximum gain
  T fastK;              ///< Factor for increasing gain
  T slowK;              ///< Factor for reducing gain
  T peakK;              ///< Factor for increasing peak gain

  // AGC state 
  T gain;               ///< Current gain
  T peak;               ///< Current estimate of the signal peak

  // Debugging
  bool debug;

  // Prevent copying
  AutomaticGainControl(AutomaticGainControl&);
  AutomaticGainControl& operator=(AutomaticGainControl&);
};

#endif


