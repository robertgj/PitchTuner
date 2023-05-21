/// \file saOutputFileSelector.cc

#include <stdexcept>
#include <utility>

#include "saOutputSink.h"
#include "saOutputFileSelector.h"

namespace SimpleAudio
{
  template <template<typename, class> class ThreadingModel, class MutexPolicy>
  bool saOutputFileSelector<ThreadingModel, MutexPolicy>::
  RegisterFileType(const std::string& suffix, 
                   saOutputSinkOpenFileCallBack openFunction)
  {
    typename ThreadingModel<bool, MutexPolicy>::Guard guard;
      
    if (openFunction == 0)
      {
        std::string suf(suffix.begin(), suffix.end());
        throw(std::runtime_error("Null handler for " + suf));
      }
    std::pair<saCallBackMap::iterator, bool> ret = 
      callBackMap.insert(make_pair(suffix, openFunction));
    return ret.second;
  }

  template <template<typename, class> class ThreadingModel, class MutexPolicy>
  saOutputSink* saOutputFileSelector<ThreadingModel, MutexPolicy>::
  OpenFile(const std::string& name,
           const std::size_t sampleRate,
           const std::size_t channels)

  {
    typename ThreadingModel<bool, MutexPolicy>::Lock guard;
      
    std::string::size_type suffixPos = name.find_last_of('.');
    if (suffixPos == std::string::npos)
      {
        throw(std::runtime_error("No suffix for " + 
                                 std::string(name.begin(), name.end())));
      }
    saCallBackMap::const_iterator iter = 
      callBackMap.find(name.substr(suffixPos));
    if (iter == callBackMap.end())
      {
        throw(std::runtime_error("No handler for " + 
                                 std::string(name.begin(), name.end())));
      }
    return (iter->second)(name, sampleRate, channels);
  }

  // Specialize the template to help the compiler
  template class saOutputFileSelector<ThreadingModel::SingleThreaded, 
                                     MutexPolicy::NoMutex>;

  saOutputSink* saOutputSinkOpenFile(const std::string& fileName,
                                     const std::size_t sampleRate,
                                     const std::size_t channels)

  {
    return saSingletonOutputFileSelector::Instance().
      OpenFile(fileName, sampleRate, channels);
  }
}
