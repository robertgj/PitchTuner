/// \file SingletonHolder.h
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

// StdC headers
#include <cassert>

// Local headers
#include "CreationPolicy.h"
#include "LifetimePolicy.h"
#include "MutexPolicy.h"
#include "ThreadingModel.h"

#ifndef __SINGLETONHOLDER_H__
#define __SINGLETONHOLDER_H__

namespace SingletonHolder
{

  ////////////////////////////////////////////////////////////////////
  /// SingletonHolder class declaration
  ////////////////////////////////////////////////////////////////////

  /// SingletonHolder class declaration
  template
  <
    typename T,
    template <class> class CreationPolicy = CreationPolicy::CreateUsingNew,
    template <class> class LifetimePolicy = LifetimePolicy::DefaultLifetime,
    template <typename, class> class ThreadingModel = 
    ThreadingModel::SingleThreaded,
    class MutexPolicy = MutexPolicy::NoMutex
    >
  class SingletonHolder
  {
  public:
    ///  Type of the singleton object
    typedef T ObjectType;

    ///  Returns a reference to singleton object
    static T& Instance();
    
  private:
    /// Helpers
    static void MakeInstance();
    static void DestroySingleton();
    
    /// Protection
    SingletonHolder();
    
    /// Data
    typedef 
    typename ThreadingModel<T*, MutexPolicy>::VolatileType PtrInstanceType;
    static PtrInstanceType pInstance_;
    static bool destroyed_;
  };

  ////////////////////////////////////////////////////////////////////
  /// SingletonHolder member functions
  ////////////////////////////////////////////////////////////////////

  /// Return a reference to the singleton instance
  template
  <
    typename T,
    template <class> class CreationPolicy,
    template <class> class LifetimePolicy,
    template <typename, class> class ThreadingModel,
    class MutexPolicy
    >
  T& SingletonHolder<T, 
                            CreationPolicy,
                            LifetimePolicy, 
                            ThreadingModel, 
                            MutexPolicy>::Instance()
  {
    if (!pInstance_) { MakeInstance(); }
    return *pInstance_;
  }
  
  /// Make a singleton instance with thread safety
  template
  <
    typename T,
    template <class> class CreationPolicy,
    template <class> class LifetimePolicy,
    template <typename, class> class ThreadingModel,
    class MutexPolicy
    >
  void SingletonHolder<T, 
                       CreationPolicy,
                       LifetimePolicy, 
                       ThreadingModel, 
                       MutexPolicy>::MakeInstance()
  {
    typename ThreadingModel<SingletonHolder,MutexPolicy>::Lock guard;
    
    if (!pInstance_)
      {
        if (destroyed_)
          {
            destroyed_ = false;
            LifetimePolicy<T>::OnDeadReference();
          }
        pInstance_ = CreationPolicy<T>::Create();
        LifetimePolicy<T>::ScheduleDestruction(pInstance_, &DestroySingleton);
      }
  }

  /// Destroy a singleton instance
  template
  <
    typename T,
    template <class> class CreationPolicy,
    template <class> class L,
    template <typename, class> class M,
    class X
    >
  void SingletonHolder<T, CreationPolicy, L, M, X>::DestroySingleton()
  {
    assert(!destroyed_);
    CreationPolicy<T>::Destroy(pInstance_);
    pInstance_ = 0;
    destroyed_ = true;
  }

  ////////////////////////////////////////////////////////////////////
  /// SingletonHolder static data
  ////////////////////////////////////////////////////////////////////

  /// Singleton holder instance
  template
  <
    typename T,
    template <class> class C,
    template <class> class L,
    template <typename, class> class M,
    class X
    >
  typename SingletonHolder<T, C, L, M, X>::PtrInstanceType
  SingletonHolder<T, C, L, M, X>::pInstance_;

  /// Flag to indicate singleton has been destroyed
  template
  <
    typename T,
    template <class> class C,
    template <class> class L,
    template <typename, class> class M,
    class X
    >
  bool SingletonHolder<T, C, L, M, X>::destroyed_;
    
} // Private

#endif // __SINGLETONHOLDER_H__
