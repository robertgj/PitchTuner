/// \file saSampleTest.h
/// \brief Public interface for testing a fixed point audio sample.

#include <cstdint>

#include "saSampleBase.h"

#ifndef __SASAMPLETEST_H__
#define __SASAMPLETEST_H__

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

