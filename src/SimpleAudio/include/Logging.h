/// \file : Logging.h
//
// Code from the article "Logging In C++: A typesafe, threadsafe, 
// portable logging mechanism" by Petru Marginean, DDJ, September 05, 2007
//
// Example:
//
// #include "Logging.h"
//
// using namespace Logging;
//
// int main(int argc, char* argv[])
// {
//   try
//     {
//       FILELog::ReportingLevel() = 
//         FILELog::FromString(argv[1] ? argv[1] : "DEBUG");
//       const int count = 3;
//       FILE_LOG(logDEBUG) << "A loop with " << count << " iterations";
//       for (int i = 0; i != count; ++i)
//         {
//           FILE_LOG(logDEBUG) << "the counter i = " << i;
//         }
//       return 0;
//     }
//   catch(const std::exception& e)
//     {
//       FILE_LOG(logERROR) << e.what();
//     }
//   return -1;
// }
//

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <sstream>
#include <string>
#include <stdio.h>

namespace Logging
{

  std::string NowTime();

  enum LoggingLevel { logERROR, logWARNING, logINFO, logDEBUG };

  template <typename LogOutputPolicy>
  class Log
  {
  public:
    Log();
    virtual ~Log();
    std::wostringstream& Get(LoggingLevel level = logINFO);
  public:
    static LoggingLevel& ReportingLevel();
    static std::string ToString(LoggingLevel level);
    static LoggingLevel FromString(const std::string& level);
  protected:
    std::wostringstream os;
  private:
    Log(const Log&);
    Log& operator =(const Log&);
  };

  template <typename LogOutputPolicy>
  Log<LogOutputPolicy>::Log()
  {
  }

  template <typename LogOutputPolicy>
  std::wostringstream& Log<LogOutputPolicy>::Get(LoggingLevel level)
  {
    os << "- " << NowTime();
    os << " " << ToString(level) << ": ";
    return os;
  }

  template <typename LogOutputPolicy>
  Log<LogOutputPolicy>::~Log()
  {
    os << std::endl;
    LogOutputPolicy::Output(os.str());
  }

  template <typename LogOutputPolicy>
  LoggingLevel& Log<LogOutputPolicy>::ReportingLevel()
  {
    static LoggingLevel reportingLevel = logDEBUG;
    return reportingLevel;
  }

  template <typename LogOutputPolicy>
  std::string Log<LogOutputPolicy>::ToString(LoggingLevel level)
  {
    static const char* const buffer[] = { "ERROR", "WARNING", "INFO", "DEBUG" };
    return buffer[level];
  }

  template <typename LogOutputPolicy>
  LoggingLevel Log<LogOutputPolicy>::FromString(const std::string& level)
  {
    if (level == "DEBUG")
      return logDEBUG;
    if (level == "INFO")
      return logINFO;
    if (level == "WARNING")
      return logWARNING;
    if (level == "ERROR")
      return logERROR;
    Log<LogOutputPolicy>().Get(logWARNING) << "Unknown logging level '" 
                                           << level 
                                           << "'. Using INFO level as default.";
    return logINFO;
  }

  class Output2FILE
  {
  public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
  };

  FILE*& Output2FILE::Stream()
  {
    static FILE* pStream = stderr;
    return pStream;
  }

  void Output2FILE::Output(const std::string& msg)
  {   
    FILE* pStream = Stream();
    if (!pStream)
      return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
  }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   if defined (BUILDING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllexport)
#   elif defined (USING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllimport)
#   else
#       define FILELOG_DECLSPEC
#   endif // BUILDING_DBSIMPLE_DLL
#else
#   define FILELOG_DECLSPEC
#endif // _WIN32

  class FILELOG_DECLSPEC FILELog : public Log<Output2FILE> {};

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logDEBUG
#endif

#define FILE_LOG(level)                                                   \
  if (level > FILELOG_MAX_LEVEL) ;                                        \
  else if (level > FILELog::ReportingLevel() || !Output2FILE::Stream()) ; \
  else FILELog().Get(level)

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

  std::string NowTime()
  {
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, 
                       "HH':'mm':'ss", buffer, MAX_LEN) == 0)
      return "Error in NowTime()";

    char result[101] = {0};
    static DWORD first = GetTickCount();
    snprintf(result, 100, 
             "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000); 
    return result;
  }

#else

#include <sys/time.h>

  std::string NowTime()
  {
    char buffer[11];
    time_t t;
    time(&t);
    tm r;
    memset(&r, 0, sizeof(tm));
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[101] = {0};
    snprintf(result, 100, "%s.%03ld", buffer, (long)tv.tv_usec / 1000); 
    return result;
  }

} // namespace Logging

#endif //WIN32

#endif //__LOGGING_H__

