/// \file saSample.cc
/// \brief Template specialisation of an integer or floating point audio sample

#include <iostream>

#include "saSample.h"

namespace SimpleAudio
{
  /// Implementation of <<
  template <> std::ostream& operator<<(std::ostream& os, const saSample& rhs)
  { 
    float val = rhs.to<float>();
    os << val;
    return os;
  }
}
