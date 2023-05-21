/// \file NonCopyable.h
///
//  Private copy constructor and copy assignment ensure 
//  classes derived from class noncopyable cannot be copied.


#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace NonCopyable
{

  class noncopyable
  {
   protected:
      noncopyable() {}
      ~noncopyable() {}
   private:  // emphasize the following members are private
      noncopyable( const noncopyable& );
      const noncopyable& operator=( const noncopyable& );
  };

}

#endif // __NONCOPYABLE_H__
