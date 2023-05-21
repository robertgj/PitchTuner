/// \file saInputFileSelector.cc

#include <stdexcept>
#include <string>
#include <utility>

#include "saInputSource.h"
#include "saInputFileSelector.h"

namespace SimpleAudio
{
  template <template<typename, class> class ThreadingModel, class MutexPolicy>
  bool saInputFileSelector<ThreadingModel, MutexPolicy>::
  RegisterFileType(const std::string& suffix, 
                   saInputSourceOpenFileCallBack openFunction)
  {
    typename ThreadingModel<bool, MutexPolicy>::Guard guard;
      
    if (openFunction == 0)
      {
        std::string suf(suffix.begin(), suffix.end());
		throw(std::runtime_error(std::string("Null handler for ") + suf));
      }
    std::pair<saCallBackMap::iterator, bool> ret = 
      callBackMap.insert(make_pair(suffix, openFunction));
    return ret.second;
  }

  template <template<typename, class> class ThreadingModel, class MutexPolicy>
  saInputSource* saInputFileSelector<ThreadingModel, MutexPolicy>::
  OpenFile(const std::string& name)
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
    return (iter->second)(name);
  }

  // Specialize the template to help the compiler
  template class saInputFileSelector<ThreadingModel::SingleThreaded, 
                                     MutexPolicy::NoMutex>;

  saInputSource* saInputSourceOpenFile(const std::string& fileName)
  {
    return saSingletonInputFileSelector::Instance().OpenFile(fileName);
  }
}
