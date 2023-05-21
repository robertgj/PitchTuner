// \file saSampleTest_test.cc
//
#include <iostream>
#include <iomanip>

#include <csignal>

#include "saSampleTest.h"

using namespace SimpleAudio;

static void abortHandler(int sig)
{
  std::cout << "Trapped signal " << sig << std::endl ;
}

int
main(void)
{
  // Setup
  std::cout << std::setprecision(6);
  if (signal(SIGABRT, abortHandler) == SIG_ERR)
    {
      std::cout << "signal() failed" << std::endl ;
      throw;
    }

  // Test constructors
  std::cout << "Test constructors" << std::endl ; 
  
  std::cout << "saInputMax=" << saSampleTest::saInputMax << std::endl;
  std::cout << "saInputMin=" << saSampleTest::saInputMin << std::endl;
  std::cout << "saInternalMax=" << saSampleTest::saInternalMax << std::endl;
  std::cout << "saInternalMin=" << saSampleTest::saInternalMin << std::endl;
  std::cout << "saInternalOne=" << saSampleTest::saInternalOne << std::endl;

  saSampleTest a;
  std::cout << "Default: a=" << a << std::endl ; 

  saSampleTest *b = new saSampleTest(0);
  std::cout << "Explicitly initialise to 0 : b=" << *b << std::endl ; 
  delete b;

  saSampleTest c = 0.0;
  std::cout << "Explicitly initialise to 0.0 : c=" << c << std::endl ; 

  saSampleTest d(1);
  std::cout << "Explicitly initialise to 1 : d=" << d << std::endl ; 

  saSampleTest e(-1);
  std::cout << "Explicitly initialise to -1 : e=" << e << std::endl ; 
 
  saSampleTest* f = &e;
  std::cout << "Test addressof : f = &e : *f=" << *f << std::endl ; 

  saSampleTest g(saSampleTest::saInputMax);
  std::cout << "Explicitly initialise to "
            << saSampleTest::saInputMax
            << " : g=" 
            << g 
            << std::endl ; 
    
  saSampleTest h(saSampleTest::saInputMin);
  std::cout << "Explicitly initialise to "
            << saSampleTest::saInputMin
            << " : h=" 
            << h 
            << std::endl ; 
    
  try 
    {
      std::cout << "Explicitly initialise to " 
                << saSampleTest::saInputMax
                << "+1" 
                << std::endl ; 
      saSampleTest i(saSampleTest::saInputMax+1);
      std::cout << "i=" << i << std::endl ; 
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Explicitly initialise to " 
                << saSampleTest::saInputMin
                << "-1" 
                << std::endl ; 
      saSampleTest j(saSampleTest::saInputMin-1);
      std::cout << "j=" << j << std::endl ; 
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  saSampleTest k(1.0f*(saSampleTest::saInputMax-1));
  std::cout << "Explicitly initialise to " 
            << saSampleTest::saInputMax-1
            << " : k=" 
            << k 
            << std::endl ;

  saSampleTest l(1.0f/saSampleTest::saInternalOne);
  std::cout << "Explicitly initialise to 1.0f/"
            << saSampleTest::saInternalOne 
            <<" : l=" 
            << l 
            << std::endl ; 

  saSampleTest m(-1.0f/saSampleTest::saInternalOne);
  std::cout << "Explicitly initialise to -1.0f/"
            << saSampleTest::saInternalOne
            << " : m=" 
            << m 
            << std::endl ; 

  try 
    {
      std::cout << "Explicitly initialise to 1.0f/"
                << (saSampleTest::saInternalOne+1.0f)
                << std::endl ; 
      saSampleTest n(1.0f/(saSampleTest::saInternalOne+1.0f));
      std::cout << "n = " << n << std::endl ;
      
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Explicitly initialise to -1.0f/"
                << (saSampleTest::saInternalOne+1.0f)
                << std::endl ; 
      saSampleTest o(-1.0f/(saSampleTest::saInternalOne+1.0f));
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  saSampleTest *p = new saSampleTest(1);
  saSampleTest q = *p;
  std::cout << "Copy constructor : p = 1 : q = *p : q=" << q << std::endl ; 
  saSampleTest r(*p);
  std::cout << "Copy constructor : r(*p) : r=" << r << std::endl ; 
  delete p;

  // Test assignments
  std::cout << "Test assignments" << std::endl ; 
  
  saSampleTest s;
  s=1;
  std::cout << "Assignment : s=1 : s=" << s << std::endl ; 

  s=0.5;
  std::cout << "Assignment : s=0.5 : s=" << s << std::endl ; 
  s+=0;
  std::cout << "Assignment : s+=0 : s=" << s << std::endl ; 
  s+=1.0;
  std::cout << "Assignment : s+=1.0 : s=" << s << std::endl ; 
  s+=-2.0;
  std::cout << "Assignment : s+=-2.0 : s=" << s << std::endl ; 
  s+=1;
  std::cout << "Assignment : s+=1 : s=" << s << std::endl ; 
  s+=-2;
  std::cout << "Assignment : s+=-2 : s=" << s << std::endl ; 

  try 
    {
      std::cout << "Assignment : s="
                << saSampleTest::saInputMax
                << " : s+=1" << std::endl ; 
      s=saSampleTest::saInputMax;
      s += 1;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Assignment : s="
                << saSampleTest::saInputMax
                << " : s-=-1" << std::endl ; 
      s=saSampleTest::saInputMax;
      s -= -1;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Assignment : s="
                << saSampleTest::saInputMin
                << " : s+=-1" << std::endl ; 
      s=saSampleTest::saInputMin;
      s += -1;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Assignment : s="
                << saSampleTest::saInputMin
                << " : s-=1" << std::endl ; 
      s=saSampleTest::saInputMin;
      s -= 1;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  s=1;
  s*=2;
  std::cout << "Assignment : s=1 : s*=2 : s=" << s << std::endl ; 

  s=1.0;
  s*=2.0;
  std::cout << "Assignment : s=1.0 : s*=2.0 : s=" << s << std::endl ; 

  s=1.0;
  s*=-2;
  std::cout << "Assignment : s=1.0 : s*=-2 : s=" << s << std::endl ; 

  s=1;
  s*=-2.0;
  std::cout << "Assignment : s=1 : s*=-2.0 : s=" << s << std::endl ; 

  s=-1;
  s*=2;
  std::cout << "Assignment : s=1 : s*=2 : s=" << s << std::endl ; 

  s=-1;
  s*=2.0;
  std::cout << "Assignment : s=-1 : s*=2.0 : s=" << s << std::endl ; 

  s=-1.0;
  s*=-2;
  std::cout << "Assignment : s=-1.0 : s*=-2 : s=" << s << std::endl ; 

  s=-1.0;
  s*=-2.0;
  std::cout << "Assignment : s=-1.0 : s*=-2.0 : s=" << s << std::endl ; 

  s=1111;
  s*=3.33333 ;
  std::cout << "Assignment : s=1111; s*=3.33333 : s=" << s << std::endl ; 

  try 
    {
      std::cout << "Assignment : s=" 
                << saSampleTest::saInputMax
                << " : s*=2.0" 
                << std::endl ; 
      s=saSampleTest::saInputMax;
      s*=2.0;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Assignment : s=" 
                << saSampleTest::saInputMax
                << " : s*=-2.0" 
                << std::endl ; 
      s=saSampleTest::saInputMax;
      s*=-2.0;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Assignment : s=1.0f/" 
                << saSampleTest::saInternalOne
                << " : s/=2" 
                << std::endl ; 
      s=1.0f/saSampleTest::saInternalOne;
      s/=2;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  try 
    {
      std::cout << "Assignment : s=1/0" << std::endl ; 
      s=1;
      s/=0;
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  s=0;
  s/=2;
  std::cout << "Assignment : s=0; s/=2 : s=" << s << std::endl ; 

  s=1.0;
  s/=2;
  std::cout << "Assignment : s=1.0; s/=2 : s=" << s << std::endl ; 

  s=1.0;
  s/=2.0;
  std::cout << "Assignment : s=1.0; s/=2.0 : s=" << s << std::endl ; 

  s=-1.0;
  s/=2;
  std::cout << "Assignment : s=-1.0; s/=2 : s=" << s << std::endl ; 

  s=-1;
  s/=2.0;
  std::cout << "Assignment : s=-1; s/=2.0 : s=" << s << std::endl ; 

  s=1.0;
  s/=-2;
  std::cout << "Assignment : s=1.0; s/=-2 : s=" << s << std::endl ; 

  s=1.0;
  s/=-2.0;
  std::cout << "Assignment : s=1.0; s/=-2.0 : s=" << s << std::endl ; 

  s=-1;
  s/=-2;
  std::cout << "Assignment : s=1; s/=-2 : s=" << s << std::endl ; 

  s=-1.0;
  s/=-2.0;
  std::cout << "Assignment : s=-1.0; s/=-2.0 : s=" << s << std::endl ; 

  s=5555;
  s/=3333;
  std::cout << "Assignment : s=5555; s/=3333 : s=" << s << std::endl ; 

  s=1;
  s/=1.0;
  std::cout << "Assignment : s=1 : s/=1.0 : s=" << s << std::endl ; 

  s/=1;
  std::cout << "Assignment : s/=1 : s=" << s << std::endl ; 

  s=saSampleTest::saInputMax;
  s/=1;
  std::cout << "Assignment : s=" 
            << saSampleTest::saInputMax 
            << " ; s/=1 : s="  
            << s 
            << std::endl ; 

  // Test arithmetic
  std::cout << "Test arithmetic" << std::endl ; 

  saSampleTest t, u, v;

  t = 1;
  t = -t;
  std::cout << "Arithmetic t=-(t=1); t=" << t << std::endl ; 

  t = -1;
  t = -t;
  std::cout << "Arithmetic t=-(t=-1); t=" << t << std::endl ; 

  t = 2;
  t = -t;
  std::cout << "Arithmetic t=-(t=2); t=" << t << std::endl ; 

  t = -2;
  t = -t;
  std::cout << "Arithmetic t=-(t=-2); t=" << t << std::endl ; 

  t = 0.5;
  t = -t;
  std::cout << "Arithmetic t=-(t=0.5); t=" << t << std::endl ; 

  t = -0.5;
  t = -t;
  std::cout << "Arithmetic t=-(t=-0.5); t=" << t << std::endl ; 

  t = saSampleTest::saInputMax;
  t = -t;
  std::cout << "Arithmetic t=-(t=saInputMax); t=" << t << std::endl ; 

  try 
    {
      t = saSampleTest::saInputMin;
      std::cout << "Arithmetic t=-(t=saInputMin);" << std::endl ; 
      t = -t;
      std::cout << "t=" << t << std::endl ; 
    }
  catch(std::exception& excp)
    {
      std::cout << excp.what() << std::endl ;
    }

  t = 1;
  u = 2;
  v = t + u;
  std::cout << "Arithmetic 1+2 : v=" << v << std::endl ; 

  t = 1;
  u = -2;
  v = t + u;
  std::cout << "Arithmetic 1+(-2) : v=" << v << std::endl ; 

  t = -1;
  u = 2;
  v = t + u;
  std::cout << "Arithmetic (-1)+2 : v=" << v << std::endl ; 

  t = -1;
  u = -2;
  v = t + u;
  std::cout << "Arithmetic (-1)+(-2) : v=" << v << std::endl ; 

  v = 1;
  u = -v;
  std::cout << "Arithmetic -(1) : v=" << v << " u=" << u << std::endl ; 

  v = -1;
  u = -v;
  std::cout << "Arithmetic -(-1) : v=" << v << " u=" << u << std::endl ; 

  t = 1;
  u = 2;
  v = t - u;
  std::cout << "Arithmetic 1-2 : v=" << v << std::endl ; 

  t = 1;
  u = -2;
  v = t - u;
  std::cout << "Arithmetic 1-(-2) : v=" << v << std::endl ; 

  t = -1;
  u = 2;
  v = t - u;
  std::cout << "Arithmetic (-1)-2 : v=" << v << std::endl ; 

  t = -1;
  u = -2;
  v = t - u;
  std::cout << "Arithmetic (-1)-(-2) : v=" << v << std::endl ; 

  t = 2.0;
  u = 3.0;
  v = t * u;
  std::cout << "Arithmetic 2.0*3.0 : v=" << v << std::endl ; 

  t = 2.0;
  u = -3.0;
  v = t * u;
  std::cout << "Arithmetic 2.0*(-3.0) : v=" << v << std::endl ; 

  t = -2.0;
  u = 3.0;
  v = t * u;
  std::cout << "Arithmetic (-2.0)*3.0 : v=" << v << std::endl ; 

  t = -2.0;
  u = -3.0;
  v = t * u;
  std::cout << "Arithmetic (-2.0)*(-3.0) : v=" << v << std::endl ; 

  t = 2.0;
  u = 4.0;
  v = t / u;
  std::cout << "Arithmetic 2.0/4.0 : v=" << v << std::endl ; 

  t = 2.0;
  u = -4.0;
  v = t / u;
  std::cout << "Arithmetic 2.0/(-4.0) : v=" << v << std::endl ; 

  t = -2.0;
  u = 4.0;
  v = t / u;
  std::cout << "Arithmetic (-2.0)/4.0 : v=" << v << std::endl ; 

  t = -2.0;
  u = -4.0;
  v = t / u;
  std::cout << "Arithmetic (-2.0)/(-4.0) : v=" << v << std::endl ; 

  u = -4;
  v = 2 + u;
  std::cout << "Arithmetic 2+(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u + 2;
  std::cout << "Arithmetic (u=-4)+2 : v=" << v << std::endl ; 

  u = -4;
  v = 0.5 + u;
  std::cout << "Arithmetic 0.5+(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u + 0.5;
  std::cout << "Arithmetic (u=-4)+0.5 : v=" << v << std::endl ; 

  u = -4;
  v = 2 - u;
  std::cout << "Arithmetic 2-(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u - 2;
  std::cout << "Arithmetic (u=-4)-2 : v=" << v << std::endl ; 

  u = 4;
  v = 0.5 - u;
  std::cout << "Arithmetic 0.5-(u=4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u - 0.5;
  std::cout << "Arithmetic (u=-4)-0.5 : v=" << v << std::endl ; 

  u = -4;
  v = 2 * u;
  std::cout << "Arithmetic 2*(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u * 2;
  std::cout << "Arithmetic (u=-4)*2 : v=" << v << std::endl ; 

  u = -4;
  v = 0.5 * u;
  std::cout << "Arithmetic 0.5*(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u * 0.5;
  std::cout << "Arithmetic (u=-4)*0.5 : v=" << v << std::endl ; 

  u = -4;
  v = 2 / u;
  std::cout << "Arithmetic 2/(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u / 2;
  std::cout << "Arithmetic (u=-4)/2 : v=" << v << std::endl ; 

  u = -4;
  v = 0.5 / u;
  std::cout << "Arithmetic 0.5/(u=-4) : v=" << v << std::endl ; 
  
  u = -4;
  v = u / 0.5;
  std::cout << "Arithmetic (u=-4)/0.5 : v=" << v << std::endl ; 

  // Test comparison
  std::cout << "Test comparison" << std::endl ; 

  saSampleTest w;

  w = 0;
  if (w == 0)
    {
      std::cout << "Comparison : w == 0" << std::endl ; 
    }
  else 
    {
      std::cout << "Comparison : w == 0 failed!" << std::endl ; 
    }

  w = 1;
  if (w != 0)
    {
      std::cout << "Comparison : w != 0" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w != 0 failed!" << std::endl ; 
    }

  w = 1;    {
      std::cout << "Comparison : w < x" << std::endl ; 
    }

  if (w > 0)
    {
      std::cout << "Comparison : w > 0" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w > 0 failed!" << std::endl ; 
    }

  w = 0;
  if (w >= 0)
    {
      std::cout << "Comparison : w >= 0" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w >= 0 failed!" << std::endl ; 
    }

  w = 0;
  if (w <= 0)
    {
      std::cout << "Comparison : w <= 0" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w <= 0 failed!" << std::endl ; 
    }

  w = -1;
  if (w < 0)
    {
      std::cout << "Comparison : w < 0" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w < 0 failed!" << std::endl ; 
    }

  saSampleTest x;

  w = 0;
  x = 0;
  if (w == x)
    {
      std::cout << "Comparison : w == x" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w == x failed!" << std::endl ; 
    }

  w = 1;
  x = 0;
  if (w != x)
    {
      std::cout << "Comparison : w != x" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w != x failed!" << std::endl ; 
    }

  w = 1;
  x = 0;
  if (w > x)
    {
      std::cout << "Comparison : w > x" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w > x failed!" << std::endl ; 
    }

  w = 0;
  x = 0;
  if (w >= x)
    {
      std::cout << "Comparison : w >= x" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w >= x failed!" << std::endl ; 
    }

  w = -1;
  x = -1;
  if (w <= x)
    {
      std::cout << "Comparison : w <= x" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w <= x failed!" << std::endl ; 
    }

  w = -2;
  x = -1;
  if (w < x)
    {
      std::cout << "Comparison : w < x" << std::endl ; 
    }
  else
    {
      std::cout << "Comparison : w < x failed!" << std::endl ; 
    }

#if 0
  // Testing MemoryLeakChecker
  saSampleTest *y = new saSampleTest(0);
  *y = -100;

  saSampleTest *z = new saSampleTest[10];
  z[0] = -100;
#elif 0
  // Testing ElectricFence
  saSampleTest *z = new saSampleTest(0);
  delete z;
  *z = 100;
#endif

  return 0;
}

