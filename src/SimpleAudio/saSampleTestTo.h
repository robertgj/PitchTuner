/// \file saSampleTestTo.h
/// \brief Public interface for a simple fixed point audio sample intended
/// for testing saSampleBase.h conversion functions with an int32_t saSample.

#include <iosfwd>
#include <stdexcept>
#include <limits>

#include <limits.h>
#include <cstdint>

#include "saSampleBase.h"

#ifndef __SASAMPLETESTTO_H__
#define __SASAMPLETESTTO_H__

namespace SimpleAudio
{
  /// Define saSampleTest
  typedef saSampleBase<int32_t, int64_t, true, true, true> saSampleTest;

  /// A conversion functor
  template <typename S>
  struct saSampleTestTo
  {
    S operator()(const saSampleTest& s)
    {
      return s.to<S>();
    }
  };
}

#endif

