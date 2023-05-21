/// \file CreationPolicy.h
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

#ifndef __CREATIONPOLICY_H__
#define __CREATIONPOLICY_H__

namespace CreationPolicy
{

  ////////////////////////////////////////////////////////////////////
  /// Creation policies
  ////////////////////////////////////////////////////////////////////

  template <class T> struct CreateUsingNew
  {
    static T* Create() { return new T; }
    
    static void Destroy(T* p) { delete p; }
  };
    
} // CreationPolicy

#endif // __CREATIONPOLICY_H__
