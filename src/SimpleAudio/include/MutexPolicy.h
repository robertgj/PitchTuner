/// \file MutexPolicy.h
///

#include <memory>

// Local headers
#include "NonCopyable.h"

#ifndef __MUTEXPOLICY_H__
#define __MUTEXPOLICY_H__

namespace MutexPolicy
{
  ////////////////////////////////////////////////////////////////////
  /// Mutex policies
  ////////////////////////////////////////////////////////////////////

  /// No mutex!
  class NoMutex : NonCopyable::noncopyable
  {
  public:
    NoMutex() { }
    ~NoMutex() { }
    void Lock() { }
    void Unlock() { }
  };
}

#endif // __MUTEXPOLICY_H__

