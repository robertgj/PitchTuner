/// \file saOutputFileSelector.h

#include <map>

#include "saOutputSink.h"
#include "SingletonHolder.h"
#include "NonCopyable.h"

#ifndef __SAOUTPUTFILESELECTOR_H__
#define __SAOUTPUTFILESELECTOR_H__

namespace SimpleAudio
{
  typedef saOutputSink* 
  (*saOutputSinkOpenFileCallBack)(const std::string& name,
                                  const std::size_t sampleRate,
                                  const std::size_t channels);


  template <template<typename, class> class ThreadingModel, class MutexPolicy>
  class saOutputFileSelector : NonCopyable::noncopyable
  {
  public:
    bool RegisterFileType(const std::string& suffix, 
                          saOutputSinkOpenFileCallBack openFunction);

    saOutputSink* OpenFile(const std::string& name,
                           const std::size_t sampleRate,
                           const std::size_t channels);

  private:
    typedef std::map<std::string, saOutputSinkOpenFileCallBack> saCallBackMap;
    saCallBackMap callBackMap;
  };

  typedef SingletonHolder::SingletonHolder
  <
    saOutputFileSelector<ThreadingModel::SingleThreaded, MutexPolicy::NoMutex>,
    CreationPolicy::CreateUsingNew, 
    LifetimePolicy::DefaultLifetime,
    ThreadingModel::SingleThreaded, 
    MutexPolicy::NoMutex
    >
  saSingletonOutputFileSelector;

}

#endif 
