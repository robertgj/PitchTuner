/// \file saInputFileSelector.h

#include <map>

#include "saInputSource.h"
#include "NonCopyable.h"
#include "SingletonHolder.h"

#ifndef __SAINPUTFILESELECTOR_H__
#define __SAINPUTFILESELECTOR_H__

namespace SimpleAudio
{
  typedef saInputSource* 
  (*saInputSourceOpenFileCallBack)(const std::string& name);

  template <template<typename, class> class ThreadingModel, class MutexPolicy>
  class saInputFileSelector : NonCopyable::noncopyable
  {
  public:
    bool RegisterFileType(const std::string& suffix, 
                          saInputSourceOpenFileCallBack openFunction);

    saInputSource* OpenFile(const std::string& name);

  private:
    typedef std::map<std::string, saInputSourceOpenFileCallBack> saCallBackMap;
    saCallBackMap callBackMap;
  };

  typedef SingletonHolder::SingletonHolder
  <
    saInputFileSelector<ThreadingModel::SingleThreaded, MutexPolicy::NoMutex>,
    CreationPolicy::CreateUsingNew, 
    LifetimePolicy::DefaultLifetime,
    ThreadingModel::SingleThreaded, 
    MutexPolicy::NoMutex
    >
  saSingletonInputFileSelector;

}

#endif 
