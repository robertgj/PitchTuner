// \file saCircBuff_test.cc
//

// StdC++ include files
#include <chrono>
#include <iostream>
#include <iomanip>
#include <memory>
#include <thread>
#include <semaphore>
#include <stdexcept>
#include <cstdint>
#include <cstring>

using namespace std::chrono_literals;
using binary_semaphore = std::counting_semaphore<1>;

// Local include files
#include "saSample.h"
#include "saCircBuff.h"

using namespace SimpleAudio;

typedef saCircBuff<int16_t,
                   int16_t,
                   int16_t,
                   saCircBuffCopyByteWise<int16_t>,
                   saCircBuffCopyByteWise<int16_t>,
                   true,
                   true>
saCircBuff_16_16_Def_T_T;
    
typedef saCircBuff<int16_t,
                   int16_t,
                   int16_t,
                   saCircBuffCopyByteWise<int16_t>,
                   saCircBuffCopyByteWise<int16_t>,
                   false,
                   false>
saCircBuff_16_16_Def_F_F;

typedef saCircBuff<int16_t,
                   int16_t,
                   int32_t,
                   saCircBuffCopyByteWise<int16_t>,
                   saCircBuffCopyImplicit<int16_t,int32_t>,
                   true,
                   true>
saCircBuff_16_32_Imp_T_T;

typedef saCircBuff<int16_t,
                   saSample,
                   int32_t,
                   saCircBuffCopyExplicit<saSample,int16_t,saSampleTo<int16_t>>,
                   saCircBuffCopyImplicit<int16_t,int32_t>,
                   false,
                   true>
saCircBuff_16_saSample_32_Exp_F_T;

template < typename C, typename T>
void producer_fn(C& cbuf, binary_semaphore& bs, std::size_t sz, T * inbuf)
{
  static std::size_t ip = 0;
  static std::size_t total_num_written = 0;
  static std::size_t num_written;

  for (std::size_t k=0;k<10000;k++)
    {
      while(1)
        {
          std::size_t num_to_write = sz/3;
          if (num_to_write+ip > sz)
            {
              num_to_write = sz-ip;
            }
          try
            {
              num_written = cbuf.Insert(num_to_write, &(inbuf[ip]));
            }
          catch(std::logic_error &ex)
            {
              std::cout << "Caught logic error in saCircBuff::Insert()"
                        << std::endl;
              std::cout << ex.what() << std::endl;
              std::exception_ptr p = std::current_exception();
              std::rethrow_exception (p);
            }
          catch(...)
            {
              num_written = 0;
            }
          if (num_written > 0)
            {
              total_num_written += num_written;
              ip+=num_written;
              if (ip >= sz)
                {
                  ip -= sz;
                }
              bs.release();
              break;
            }
          std::this_thread::sleep_for(10us);
        }
    }
}

template < typename C, typename T>
void consumer_fn
  (C& cbuf, binary_semaphore& bs, T * outbuf, std::size_t sz, double magic)
{
  static std::size_t total_num_read = 0;
  bool timed_out = false;

  while(timed_out == false)
    {
      std::size_t num_read = 0;
      std::size_t this_read;
      do
        {
          std::size_t num_to_read = sz/4;
          if (num_to_read+num_read > sz)
            {
              num_to_read = sz-num_read;
            }
          try
            {
              this_read = cbuf.Extract(&(outbuf[num_read]), num_to_read);
            }
          catch(std::logic_error &ex)
            {
              std::cout << "Caught logic error in saCircBuff::Extract()"
                        << std::endl;
              std::cout << ex.what() << std::endl;
              std::exception_ptr p = std::current_exception();
              std::rethrow_exception (p);
            }
          catch(...)
            {
              this_read = 0;
            }
          if (this_read == 0)
            {
              if (bs.try_acquire_for(10ms) == true)
                {
                  continue;
                }
              else    
                {
                  timed_out = true;
                  break;
                }
            }
          num_read += this_read;
        } while(num_read < sz);
      total_num_read += num_read;

      if (num_read > 0)
        {
          for (std::size_t m=0;m<(num_read-1);m++)
            {
              double diff = ((double)outbuf[m+1]-(double)outbuf[m]);
              if ((diff != 1.0) && (diff != magic))
                {
                  std::cout << "Unexpected difference : " << diff << std::endl;
                }
            }
        }
#if 0
      if (num_read > 0)
        {
          std::cout << "total_num_read : " << total_num_read
                    << " num_read : " << num_read << std::endl;
          for (std::size_t m= 0; m < num_read; m++)
            {
              if ((m>0) && (m%16 == 0))
                {
                  std::cout << std::endl;
                }
              std::cout << outbuf[m] << " ";
            }
          std::cout << std::endl;
          std::cout << "total_num_read : " << total_num_read << std::endl;
        }
#endif
      
    };
}

void test_instantiation(void)
{
  std::cout << "Instantiating saCircBuff<char>(123)" << std::endl;
  try
    {
      saCircBuff<char> charBuff(123); 
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in saCircBuff<char> constructor"
                << std::endl;
      std::cout << ex.what() << std::endl;
    }

  std::cout << "Instantiating saCircBuff<int8_t>(123)" << std::endl;
  try
    {
      saCircBuff<int8_t> int8Buff(123); 
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in saCircBuff<int8_t> constructor"
                << std::endl;
      std::cout << ex.what() << std::endl;
    }
  std::cout << "Success!" << std::endl;
}

void test_nullptr(void)
{
  std::cout << "Testing nullptr argument" << std::endl;

  saCircBuff<int32_t> int32Buff(0);

  try
    {
      int32_t *bufnull = nullptr;
      int32Buff.Insert(0,bufnull);
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in bufnull" << std::endl;
      std::cout << ex.what() << std::endl;
    }
  
  try
    {
      int32_t *bufnull = nullptr;
      int32Buff.Extract(bufnull);
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in bufnull" << std::endl;
      std::cout << ex.what() << std::endl;
    }

  std::cout << "Success!" << std::endl;
}

void test_int16_write_and_int16_read(void)
{
  std::cout << "Testing  saCircBuff<int16_t>(100)" << std::endl;

  saCircBuff_16_16_Def_T_T buf1(1);

  constexpr std::size_t sz = 100;
  int16_t inbuf[sz];
  for (std::size_t k=0; k<sz; k++)
    {
      inbuf[k] = static_cast<int16_t>(k+10);
    }

  try
    {
      std::cout << "num_written: " << buf1.Insert(sz, inbuf) << std::endl;
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in buf1" << std::endl;
      std::cout << ex.what() << std::endl;
    }

  buf1.Insert(1, inbuf);
  
  // sz+1 to prevent a false positive warning from g++-13.1.1
  int16_t outbuf[sz+1];
  try
    {
      std::cout << "num_read: " << buf1.Extract(outbuf, sz) << std::endl;
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in buf1" << std::endl;
      std::cout << ex.what() << std::endl;
    }
  
  try
    {
      std::cout << "num_read: " << buf1.Extract(outbuf, 1) << std::endl;
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in buf1" << std::endl;
      std::cout << ex.what() << std::endl;
    }
  std::cout << "Success! Read " << outbuf[0] << std::endl;
    
  saCircBuff_16_16_Def_T_T bufsz(sz);
  try
    {
      std::cout << "num_written: " << bufsz.Insert(sz, inbuf) << std::endl;
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in bufsz" << std::endl;
      std::cout << ex.what() << std::endl;
    }

  try
    {
      bufsz.Insert(1, inbuf);
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in bufsz" << std::endl;
      std::cout << ex.what() << std::endl;
    }
    
  bufsz.Clear();
    
  try
    {
      std::cout << "num_read: " << bufsz.Extract(outbuf, sz+1) << std::endl;
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in bufsz after Clear()" << std::endl;
      std::cout << ex.what() << std::endl;
    }

  bufsz.Insert(sz,inbuf);
  try
    {
      std::cout << "num_read: " << bufsz.Extract(outbuf, sz) << std::endl;
    }
  catch(std::exception &ex)
    {
      std::cout << "Caught exception in bufsz" << std::endl;
      std::cout << ex.what() << std::endl;
    }
  if (memcmp(inbuf,outbuf,sz*sizeof(uint16_t)))
    {
      std::cout << "Compare 100 failed!" << std::endl;
    }
  else
    {
      std::cout << "Compare 100 success!" << std::endl;
    }

  bufsz.Clear();
  memset(outbuf, 0xaa, sz*sizeof(int16_t));
  std::size_t total_num_read = 0;
  std::size_t num_read = 0;
  std::size_t next_read = 0;
  std::size_t total_num_written = 0;
  std::size_t num_written = 0;
  std::size_t next_write = 0;

  num_written = bufsz.Insert(33,&(inbuf[next_write]));
  total_num_written += num_written;
  next_write += num_written;
  next_write = (next_write >= sz ? next_write - sz : next_write);

  num_written = bufsz.Insert(33,&(inbuf[next_write]));
  total_num_written += num_written;
  next_write += num_written;
  next_write = (next_write >= sz ? next_write - sz : next_write);

  num_read = bufsz.Extract(&(outbuf[next_read]), 25);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  num_written = bufsz.Insert(33,&(inbuf[next_write])); 
  total_num_written += num_written;
  next_write += num_written;
  next_write = (next_write >= sz ? next_write - sz : next_write);

  num_read = bufsz.Extract(&(outbuf[next_read]), 16);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  num_read = bufsz.Extract(&(outbuf[next_read]), 25);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  num_written = bufsz.Insert(1,&(inbuf[next_write])); 
  total_num_written += num_written;
  next_write += num_written;
  next_write = (next_write >= sz ? next_write - sz : next_write);

  num_written = bufsz.Insert(14,&(inbuf[next_write])); 
  total_num_written += num_written;
  next_write += num_written;
  next_write = (next_write >= sz ? next_write - sz : next_write);

  num_read = bufsz.Extract(&(outbuf[next_read]), 25);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  num_read = bufsz.Extract(&(outbuf[next_read]), 8);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  num_read = bufsz.Extract(&(outbuf[next_read]), 1);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  num_read = bufsz.Extract(&(outbuf[next_read]), 14);
  total_num_read += num_read;
  next_read += num_read;
  next_read = (next_read >= sz ? next_read - sz : next_read);

  std::cout << "total_num_written = " << total_num_written << std::endl;
  std::cout << "total_num_read = " << total_num_read << std::endl;

  if (memcmp(inbuf,outbuf,sz*sizeof(uint16_t)))
    {
      std::cout << "Compare 33 failed!" << std::endl;
    }
  else
    {
      std::cout << "Compare 33 success!" << std::endl;
    }
}

void test_threaded_int16_write_and_int16_read(void)
{
  std::cout << "Testing threaded int16_t write and int16_t read " << std::endl;
  constexpr std::size_t sz=31;
  int16_t inbuf[sz];
  for (std::size_t k=0; k<sz; k++)
    {
      inbuf[k] = static_cast<int16_t>(k+10);
    }
  
  saCircBuff_16_16_Def_F_F cbuf(sz-2);
  binary_semaphore bs(0);
  
  std::thread producer(producer_fn<saCircBuff_16_16_Def_F_F,int16_t>,
                       std::ref(cbuf),
                       std::ref(bs),
                       std::ref(sz),
                       std::ref(inbuf));

  constexpr std::size_t sz_on_2=sz/2;
  int16_t outbuf[sz_on_2];
  for (std::size_t k=0; k<sz_on_2; k++)
    {
      outbuf[k] = 77;
    }
  
  std::thread consumer(consumer_fn<saCircBuff_16_16_Def_F_F,int16_t>,
                       std::ref(cbuf),
                       std::ref(bs),
                       std::ref(outbuf),
                       std::ref(sz_on_2),
                       double(inbuf[0]) - double(inbuf[sz-1]));

  producer.join();
  consumer.join();
}

void test_threaded_int16_write_and_int32_read(void)
{
  std::cout << "Testing threaded int16_t write and int32_t read " << std::endl;
  constexpr std::size_t sz=47;
  int16_t inbuf[sz];
  for (std::size_t k=0; k<sz; k++)
    {
      inbuf[k] = static_cast<int16_t>(k+10);
    }
  
  saCircBuff_16_32_Imp_T_T cbuf(sz-2);
  binary_semaphore bs(0);
  
  std::thread producer(producer_fn<saCircBuff_16_32_Imp_T_T,int16_t>,
                       std::ref(cbuf),
                       std::ref(bs),
                       std::ref(sz),
                       std::ref(inbuf));

  constexpr std::size_t sz_on_2=sz/2;
  int32_t outbuf[sz_on_2];
  for (std::size_t k=0; k<sz_on_2; k++)
    {
      outbuf[k] = 88;
    }
  
  std::thread consumer(consumer_fn<saCircBuff_16_32_Imp_T_T,int32_t>,
                       std::ref(cbuf),
                       std::ref(bs),
                       std::ref(outbuf),
                       std::ref(sz_on_2),
                       double(inbuf[0]) - double(inbuf[sz-1]));

  producer.join();
  consumer.join();
}

void test_threaded_saSample_write_and_int32_read(void)
{
  std::cout << "Testing threaded saSample write and int32_t read " << std::endl;
  constexpr std::size_t sz=61;
  saSample inbuf[sz];
  for (std::size_t k=0; k<sz; k++)
    {
      inbuf[k] = static_cast<saSample>(k+10);
    }
  
  saCircBuff_16_saSample_32_Exp_F_T cbuf(sz-2);
  binary_semaphore bs(0);
  
  std::thread producer(producer_fn<saCircBuff_16_saSample_32_Exp_F_T,saSample>,
                       std::ref(cbuf),
                       std::ref(bs),
                       std::ref(sz),
                       std::ref(inbuf));

  constexpr std::size_t sz_on_2=sz/2;
  int32_t outbuf[sz_on_2];
  for (std::size_t k=0; k<sz_on_2; k++)
    {
      outbuf[k] = 99;
    }
  
  std::thread consumer(consumer_fn<saCircBuff_16_saSample_32_Exp_F_T,int32_t>,
                       std::ref(cbuf),
                       std::ref(bs),
                       std::ref(outbuf),
                       std::ref(sz_on_2),
                       (inbuf[0] - inbuf[sz-1]).to<double>());

  producer.join();
  consumer.join();
}

int
main(void)
{
  test_instantiation();
  test_nullptr();
  test_int16_write_and_int16_read();
  test_threaded_int16_write_and_int16_read(); 
  test_threaded_int16_write_and_int32_read();
  test_threaded_saSample_write_and_int32_read();
  
  return 0;
}
