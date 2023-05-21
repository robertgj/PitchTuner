/// \file saWavHeader.cc

#include <algorithm>
#include <functional>

#include "saWavHeader.h"

namespace SimpleAudio
{

  void saSwap2Bytes(char *buf)
  {
    char tmp[2];
    tmp[0] = buf[0]; tmp[1] = buf[1];
    buf[0] = tmp[1]; buf[1] = tmp[0];
  }

  void saSwap4Bytes(char *buf)
  {
    char tmp[4];
    tmp[0] = buf[0]; tmp[1] = buf[1]; tmp[2] = buf[2]; tmp[3] = buf[3];
    buf[0] = tmp[3]; buf[1] = tmp[2]; buf[2] = tmp[1]; buf[3] = tmp[0];
  }

  void saSwapShort(uint16_t& buffer)
  {
    saSwap2Bytes((char*)&buffer);
  }

  void saSwapShort(int16_t& buffer)
  {
    saSwap2Bytes((char*)&buffer);
  }

  void saSwapShort(int16_t* buffer, std::size_t shorts)
  {
    for (std::size_t i = 0; i<shorts; ++i)
      {
        saSwapShort(buffer[i]);
      }
  }

  void saSwapLong(int32_t& buffer)
  {
    saSwap4Bytes((char*)&buffer);
  }

  void saSwapLong(uint32_t& buffer)
  {
    saSwap4Bytes((char*)&buffer);
  }

  // Endian-ness test
  bool saIsLittleEndian()
  {
    const uint16_t endianTest = 1;
    return ( *(const char*)(&endianTest) == 1 );
  }

  bool saCompareID(char ID[4], std::string Str)
  {
    return (Str.length() == 4) &&
      (ID[0] == Str[0]) && (ID[1] == Str[1]) && 
      (ID[2] == Str[2]) && (ID[3] == Str[3]);
  }

  void saSwapWavHeader(saWavHeader* wavHeader)
  {
    saSwapLong(wavHeader->RiffChunkSize);
    saSwapLong(wavHeader->FormatChunkSize);
    saSwapShort(wavHeader->FormatTag);
    saSwapShort(wavHeader->FormatChannels);
    saSwapLong(wavHeader->FormatSamplesPerSec);
    saSwapLong(wavHeader->FormatAvgBytesPerSec);
    saSwapShort(wavHeader->FormatBlockAlign);
    saSwapShort(wavHeader->FormatBitsPerSample);
    saSwapLong(wavHeader->DataChunkSize);
  }

}
