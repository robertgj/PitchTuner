/// \file ThreadingModel.h
///
//  This is part of a simplified version of loki-0.1.6:
 
////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// StdC++ headers
#include <stdexcept>

// StdC headers
#include <cstdlib>
#include <cassert>

// Local headers
#include "NonCopyable.h"
#include "MutexPolicy.h"

#ifndef __THREADINGMODEL_H__
#define __THREADINGMODEL_H__

namespace ThreadingModel
{
  ////////////////////////////////////////////////////////////////////
  /// Threading policies
  ////////////////////////////////////////////////////////////////////

  template <typename T, class MutexPolicy = MutexPolicy::NoMutex>
  class SingleThreaded
  {
  public:
    struct Lock
    {
      Lock() {}
      explicit Lock(const SingleThreaded&) {}
      explicit Lock(const SingleThreaded*) {}
    };
    typedef T VolatileType;

    /// Convenience for scoped lock
    typedef typename SingleThreaded<T, MutexPolicy>::Lock Guard;
  };

} // ThreadingModel

#endif // __THREADINGMODEL_H__

