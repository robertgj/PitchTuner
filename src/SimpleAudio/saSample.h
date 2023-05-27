/// \file saSample.h
/// \brief Public interface for a floating point audio sample.

#include <cstdint>

#include "saDeviceType.h"

#ifndef __SASAMPLE_H__
#define __SASAMPLE_H__

#include "saSampleBase.h"

namespace SimpleAudio
{
  /// \typedef saSample
  /// The usual saSample, derived from saSampleBase
  typedef saSampleBase<float, double, true, false, false> saSample;

  /// \struct saSampleTo
  /// A conversion functor
  template <typename C>
  struct saSampleTo
  {
    C operator()(const saSample& s)
    {
      return s.to<C>();
    }
  };

  /// \struct saSampleFromSource
  /// Normally integral types are shifted to the left of the binary point.
  /// For data sources and sinks it is more convenient to think of integral
  /// values as lying in the range [-1,1). These are the permitted conversion
  /// functors that handle source and sink values.
  /// \tparam C The audio device source data type.
  template <typename C>
  struct saSampleFromSource
  {
    saSample operator()([[maybe_unused]] const C& s)
    {
      throw std::domain_error("no source conversion exists for this type");
    }
  };

  template <>
  struct saSampleFromSource<saDeviceType>
  {
    saSample operator()(const saDeviceType& s)
    {
      saSample d;
      d.fromSource<saDeviceType>(s);
      return d;
    }
  };

  /// \struct saSampleToSink
  /// \tparam C The audio device sink data type.
  template <typename C>
  struct saSampleToSink
  {
    C operator()([[maybe_unused]] const saSample& s)
    {
      throw std::domain_error("no sink conversion exists for this type");
    }
  };

  template <>
  struct saSampleToSink<saDeviceType>
  {
    saDeviceType operator()(const saSample& s)
    {
      return s.toSink<saDeviceType>();
    }
  };

}

#endif
