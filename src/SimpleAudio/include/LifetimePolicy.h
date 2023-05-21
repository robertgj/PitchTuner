/// \file LifetimePolicy.h
///
//  This is a simplified version of loki-0.1.6:
 
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

#ifndef __LIFETIMEPOLICY_H__
#define __LIFETIMEPOLICY_H__

namespace LifetimePolicy
{

  ////////////////////////////////////////////////////////////////////
  /// Lifetime policies
  ////////////////////////////////////////////////////////////////////

  /// Helper typedef for atexit()
  typedef void (*atexit_pfn_t)();

  /// Default lifetime policy
  template <class T> struct DefaultLifetime
  {
    /// Schedule object destruction 
    //
    // Schedule object destruction 
    //
    static void ScheduleDestruction(T*, atexit_pfn_t pFun)
    { if (std::atexit(pFun))throw std::runtime_error("atexit() failed!"); }
    
    static void OnDeadReference()
    { throw std::logic_error("dead reference detected"); }
  };

} // LifetimePolicy

#endif // __LIFETIMEPOLICY_H__
