// \file saSampleTestTo_test.cc
//
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <csignal>

#include "saSampleTestTo.h"

using namespace SimpleAudio;

static void abortHandler(int sig)
{
  std::cout << "Trapped signal " << sig << std::endl ;
}

template <typename T>
struct GenAdd
{
  T operator()() 
  {
    static T val = 0;
    val += 10000;
    std::cout << val << std::endl;
    return val;
  }
};

template <typename T>
struct GenMinus
{
  T operator()() 
  {
    static T val = 0;
    val -= 10000;
    std::cout << val << std::endl;
    return val;
  }
};

template <typename T>
struct GenDiv
{
  T operator()() 
  {
    static T val = 100;
    val /= 2;
    std::cout << val << std::endl;
    return val;
  }
};

template <typename T>
struct GenTimes
{
  T operator()() 
  {
    static T val = 128;
    val *= 2;
    std::cout << val << std::endl;
    return val;
  }
};

int
main(void)
{
  // Setup
  std::cout << std::setprecision(8);
  if (signal(SIGABRT, abortHandler) == SIG_ERR)
    {
      std::cout << "signal() failed" << std::endl ;
      throw;
    }

  // Try conversion functors

  // GenAdd
  std::vector<saSampleTest> i;
  try
    {
      std::generate_n(std::back_inserter(i), 7, GenAdd<saSampleTest>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }
  std::vector<int16_t> o;
  try
    {
      std::transform(i.begin(), i.end(), 
                     std::back_inserter(o), 
                     saSampleTestTo<int16_t>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }

  // GenMinus
  i.clear();
  try
    {
      std::generate_n(std::back_inserter(i), 7, GenMinus<saSampleTest>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }
  o.clear();
  try
    {
      std::transform(i.begin(), i.end(), 
                     std::back_inserter(o), 
                     saSampleTestTo<int16_t>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }

  // toFloat
  i.clear();
  try
    {
      std::generate_n(std::back_inserter(i), 10, GenDiv<saSampleTest>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }
  std::vector<float> of;
  try
    {
      std::transform(i.begin(), i.end(), 
                     std::back_inserter(of), 
                     saSampleTestTo<float>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }

  // toInt32
  i.clear();
  try
    {
      std::generate_n(std::back_inserter(i), 10, GenTimes<saSampleTest>());
      std::vector<int32_t> o32;
      std::transform(i.begin(), i.end(), 
                     std::back_inserter(o32), 
                     saSampleTestTo<int32_t>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }

  // toInt16
  i.clear();
  try
    {
      std::generate_n(std::back_inserter(i), 12, GenDiv<saSampleTest>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }
  std::vector<int16_t> o16;
  try
    {
      std::transform(i.begin(), i.end(), 
                     std::back_inserter(o16), 
                     saSampleTestTo<int16_t>());
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl;
    }

  return 0;
}
