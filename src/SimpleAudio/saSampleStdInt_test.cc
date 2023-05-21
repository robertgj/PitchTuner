/// \file saSampleStdInt_test.cc
/// \brief Simple tests to confirm type behaviour.

// (?) indicates something that I can be persuaded makes sense.

#include <limits>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <type_traits>
#include <bitset>

int main(void)
{
  std::cout << "int8_t signed : "
            << std::is_signed<int8_t>()
            << std::endl;
  
  std::cout << "int8_t digits : "
            << std::numeric_limits<int8_t>::digits
            << std::endl;

  std::bitset<8> int8_max(std::numeric_limits<int8_t>::max());
  std::cout << "int8_t max : " 
            << std::hex
            << int8_max
            << std::dec
            << "b"
            << std::endl;
  
  std::bitset<8>
    int8_min(static_cast<uint64_t>(std::numeric_limits<int8_t>::min()));
  std::cout << "int8_t min : "
            << std::hex
            << int8_min
            << std::dec    
            << "b"
            << std::endl;
  
  int8_t x=-1;
  int32_t xx=static_cast<int32_t>(x);
  std::cout << "int8_t -1 cast to int32_t : 0x"
            << std::hex
            << xx
            << std::dec    
            << std::endl;

  std::cout << "uint8_t signed : "
            << std::is_signed<uint8_t>()
            << std::endl;
  
  std::cout << "uint8_t digits : "
            << std::numeric_limits<uint8_t>::digits
            << std::endl;
  
  std::bitset<8> uint8_max(std::numeric_limits<uint8_t>::max());
  std::cout << "uint8_t max : " 
            << std::hex
            << uint8_max
            << std::dec    
            << "b"
            << std::endl;
  
  std::bitset<8> uint8_min(std::numeric_limits<uint8_t>::min());
  std::cout << "uint8_t min : "
            << std::hex
            << uint8_min
            << std::dec    
            << "b"
            << std::endl;
  
  std::cout << "int16_t signed : "
            << std::is_signed<int16_t>()
            << std::endl;
  
  std::cout << "int16_t digits : "
            << std::numeric_limits<int16_t>::digits
            << std::endl;
  
  std::cout << "int16_t max : 0x"
            << std::hex
            << std::numeric_limits<int16_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "convert int16_t max to int32_t : 0x"
            << std::hex
            << static_cast<int32_t>(std::numeric_limits<int16_t>::max())
            << std::dec 
            << std::endl;
  
  std::cout << "convert int16_t max to uint32_t : 0x"
            << std::hex
            << static_cast<uint32_t>(std::numeric_limits<int16_t>::max())
            << std::dec 
            << std::endl;
  
  std::cout << "int16_t min : 0x"
            << std::hex
            << std::numeric_limits<int16_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "convert int16_t min to int32_t : 0x"
            << std::hex
            << static_cast<int32_t>(std::numeric_limits<int16_t>::min())
            << std::dec 
            << std::endl;
  
  std::cout << "convert int16_t min to uint32_t (?) : 0x"
            << std::hex
            << static_cast<uint32_t>(std::numeric_limits<int16_t>::min())
            << std::dec 
            << std::endl;
  
  std::cout << "uint16_t signed : "
            << std::is_signed<uint16_t>()
            << std::endl;
  
  std::cout << "uint16_t digits : "
            << std::numeric_limits<uint16_t>::digits
            << std::endl;
  
  std::cout << "uint16_t max : 0x"
            << std::hex
            << std::numeric_limits<uint16_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "convert uint16_t max to int32_t : 0x"
            << std::hex
            << static_cast<int32_t>(std::numeric_limits<uint16_t>::max())
            << std::dec 
            << std::endl;
  
  std::cout << "convert uint16_t max to uint32_t : 0x"
            << std::hex
            << static_cast<uint32_t>(std::numeric_limits<uint16_t>::max())
            << std::dec 
            << std::endl;
  
  std::cout << "uint16_t min : 0x"
            << std::hex
            << std::numeric_limits<uint16_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "convert uint16_t min to int32_t : 0x"
            << std::hex
            << static_cast<int32_t>(std::numeric_limits<uint16_t>::min())
            << std::dec 
            << std::endl;
  
  std::cout << "convert uint16_t min to uint32_t : 0x"
            << std::hex
            << static_cast<uint32_t>(std::numeric_limits<uint16_t>::min())
            << std::dec 
            << std::endl;
  
  std::cout << "int32_t signed : "
            << std::is_signed<int32_t>()
            << std::endl;
  
  std::cout << "int32_t digits : "
            << std::numeric_limits<int32_t>::digits
            << std::endl;

  std::cout << "int32_t max : 0x"
            << std::hex
            << std::numeric_limits<int32_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "int32_t min : 0x"
            << std::hex
            << std::numeric_limits<int32_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "uint32_t signed : "
            << std::is_signed<uint32_t>()
            << std::endl;
  
  std::cout << "uint32_t digits : "
            << std::numeric_limits<uint32_t>::digits
            << std::endl;

  std::cout << "uint32_t max : 0x"
            << std::hex
            << std::numeric_limits<uint32_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "uint32_t min : 0x"
            << std::hex
            << std::numeric_limits<uint32_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "int64_t digits : "
            << std::numeric_limits<int64_t>::digits
            << std::endl;

  std::cout << "int64_t signed : "
            << std::is_signed<int64_t>()
            << std::endl;
  
  std::cout << "int64_t max : 0x"
            << std::hex
            << std::numeric_limits<int64_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "int64_t min : 0x"
            << std::hex
            << std::numeric_limits<int64_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "uint64_t signed : "
            << std::is_signed<uint64_t>()
            << std::endl;
  
  std::cout << "uint64_t digits : "
            << std::numeric_limits<uint64_t>::digits
            << std::endl;

  std::cout << "uint64_t max : 0x"
            << std::hex
            << std::numeric_limits<uint64_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "uint64_t min : 0x"
            << std::hex
            << std::numeric_limits<uint64_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "ssize_t signed : "
            << std::is_signed<ssize_t>()
            << std::endl;
  
  std::cout << "ssize_t digits : "
            << std::numeric_limits<ssize_t>::digits
            << std::endl;

  std::cout << "ssize_t max : 0x"
            << std::hex
            << std::numeric_limits<ssize_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "ssize_t min : 0x"
            << std::hex
            << std::numeric_limits<ssize_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "size_t signed : "
            << std::is_signed<size_t>()
            << std::endl;
  
  std::cout << "size_t digits : "
            << std::numeric_limits<size_t>::digits
            << std::endl;

  std::cout << "size_t max : 0x"
            << std::hex
            << std::numeric_limits<size_t>::max()
            << std::dec 
            << std::endl;
  
  std::cout << "size_t min : 0x"
            << std::hex
            << std::numeric_limits<size_t>::min()
            << std::dec 
            << std::endl;
  
  std::cout << "float signed : "
            << std::is_signed<float>()
            << std::endl;
  
  std::cout << "float digits : "
            << std::numeric_limits<float>::digits
            << std::endl;

  std::cout << "Set precision to 12 digits" << std::endl;
  std::cout << std::setprecision(12);
  
  std::cout << "float max : "
            << std::numeric_limits<float>::max()
            << std::endl;

  std::cout << "float min (?) : "
            << std::numeric_limits<float>::min()
            << std::endl;

  std::cout << "float -max : "
            << -std::numeric_limits<float>::max()
            << std::endl;

  std::cout << "Set precision to 40 digits" << std::endl;
  std::cout << std::setprecision(40);
  
  std::cout << "float max : "
            << std::numeric_limits<float>::max()
            << std::endl;

  std::cout << "float min (?) : "
            << std::numeric_limits<float>::min()
            << std::endl;

  std::cout << "float -max : "
            << -std::numeric_limits<float>::max()
            << std::endl;

  std::cout << "Set precision to 12 digits" << std::endl;
  std::cout << std::setprecision(12);

  std::cout << "double signed : "
            << std::is_signed<double>()
            << std::endl;

  std::cout << "double digits : "
            << std::numeric_limits<double>::digits
            << std::endl;

  std::cout << "double max : "
            << std::numeric_limits<double>::max()
            << std::endl;

  std::cout << "double min (?) : "
            << std::numeric_limits<double>::min()
            << std::endl;

  std::cout << "double -max : "
            << -std::numeric_limits<double>::max()
            << std::endl;

  static_assert(std::is_signed<int32_t>());
  
  static_assert(std::numeric_limits<double>::digits >
                2*(std::numeric_limits<float>::digits));

  return 0;
}
