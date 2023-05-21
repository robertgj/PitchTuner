/// \file saSampleTest.cc
/// \brief Template specialisation of an int32_t fixed point audio sample

#include <iostream>
#include <cstdint>

#include "saSampleTest.h"

namespace SimpleAudio
{
  /// Implementation of <<
  template <> std::ostream& operator<<(std::ostream& os, const saSampleTest& rhs)
  { 
    os << rhs.to<float>();
    return os;
  }
}
