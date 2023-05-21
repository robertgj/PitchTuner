/// \file saWavHeader.h
/// \brief WAV file format. Unfortunately I forget where I found this text:
///
/// ### RIFF header ###
/// The canonical WAVE format starts with the RIFF header:
///
/// - bytes 0 to 4,   ChunkID : Contains the letters "RIFF" in ASCII form
///                             (0x52494646 big-endian form).
/// - bytes 4 to 8, ChunkSize : 36 + SubChunk2Size, or more precisely:
///                             4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
///                             This is the size of the rest of the chunk 
///                             following this number.  This is the size of the 
///                             entire file in bytes minus 8 bytes for the
///                             two fields not included in this count:
///                             ChunkID and ChunkSize.
/// - bytes 8 to 12    Format : Contains the letters "WAVE"
///                             (0x57415645 big-endian form).
///
/// ### chunkID for Format Chunk ###
/// \code
/// typedef struct {
///   ID             chunkID;
///   long           chunkSize;
///   short          wFormatTag;
///   unsigned short wChannels;
///   unsigned long  dwSamplesPerSec;
///   unsigned long  dwAvgBytesPerSec;
///   unsigned short wBlockAlign;
///   unsigned short wBitsPerSample;
///
/// /* 
///  * Note: There may be additional fields here, 
///   *       depending upon wFormatTag. 
///  */
///
/// } FormatChunk;
/// \endcode
///
/// The ID is always "fmt ". The chunkSize field is the number of bytes in 
/// the chunk. This does not include the 8 bytes used by ID and Size fields. 
/// For the Format Chunk, chunkSize may vary according to what "format" of 
/// WAVE file is specified (i.e.: depends upon the value of wFormatTag).
///
/// WAVE data may be stored without compression, in which case the sample 
/// points are stored as described in Sample Points and Sample Frames.
/// Alternately, different forms of compression may be used when storing 
/// the sound data in the Data chunk. With compression, each sample point 
/// may take a differing number of bytes to store. The wFormatTag indicates 
/// whether compression is used when storing the data.
///
/// If compression is used (i.e.: WFormatTag is some value other than 1), then 
/// there will be additional fields appended to the Format chunk which give 
/// needed information for a program wishing to retrieve and decompress that 
/// stored data. The first such additional field will be an unsigned short 
/// that indicates how many more bytes have been appended (after this 
/// unsigned short). Furthermore, compressed formats must have a Fact chunk 
/// which contains an unsigned long indicating the size (in sample points) 
/// of the waveform after it has been decompressed. There are (too) many 
/// compressed formats. Details about them can be gotten from Microsoft's 
/// web site.
///
/// If no compression is used (i.e.: wFormatTag = 1), then there are no 
/// further fields.
///
/// The wChannels field contains the number of audio channels for the 
/// sound. A value of 1 means monophonic sound, 2 means stereo, 4 means 
/// four channel sound, etc. Any number of audio channels may be 
/// represented. For multichannel sounds, single sample points from each 
/// channel are interleaved. A set of interleaved sample points is called 
/// a sample frame.
///
/// The actual waveform data is stored in another chunk, the Data Chunk, 
/// which will be described later.
///
/// The dwSamplesPerSec field is the sample rate at which the sound is to
/// be played back in sample frames per second (i.e.: Hertz). The 3 standard
/// MPC rates are 11025, 22050, and 44100 KHz, although other rates may be
/// used.
///
/// The dwAvgBytesPerSec field indicates how many bytes play every second.
/// dwAvgBytesPerSec may be used by an application to estimate what size
/// RAM buffer is needed to properly playback the WAVE without latency
/// problems. Its value should be equal to the following formula rounded
/// up to the next whole number:
/// \code
/// dwSamplesPerSec * wBlockAlign
/// \endcode
///
/// The wBlockAlign field should be equal to the following formula, rounded
/// to the next whole number:
/// \code
/// wChannels * (wBitsPerSample / 8)
/// \endcode
///
/// Essentially, wBlockAlign is the size of a sample frame, in terms of
/// bytes. (eg, A sample frame for a 16-bit mono wave is 2 bytes. A sample
/// frame for a 16-bit stereo wave is 4 bytes. Etc).
///
/// The wBitsPerSample field indicates the bit resolution of a sample
/// point (i.e.: a 16-bit waveform would have wBitsPerSample = 16).
///
/// One, and only one, Format Chunk is required in every WAVE.
///
/// ### Data chunk ###
///
/// The Data (data) chunk contains the actual sample frames (i.e.: all 
/// channels of waveform data).
/// \code
/// typedef struct {
///   ID             chunkID;
///   long           chunkSize;
///
///   unsigned char  waveformData[];
/// } DataChunk;
/// \endcode
///
/// The ID is always data. chunkSize is the number of bytes in the chunk,
/// not counting the 8 bytes used by ID and Size fields nor any possible
/// pad byte needed to make the chunk an even size (i.e.: chunkSize is the
/// number of remaining bytes in the chunk after the chunkSize field,
/// not counting any trailing pad byte).
///
/// Remember that the bit resolution, and other information is gotten
/// from the Format chunk.
///
/// The following discussion assumes uncompressed data.
///
/// The waveformData array contains the actual waveform data. The data
/// is arranged into what are called sample frames. For more information
/// on the arrangment of data, see "Sample Points and Sample Frames".
///
/// You can determine how many bytes of actual waveform data there is
/// from the Data chunk's chunkSize field. The number of sample frames
/// in waveformData is determined by dividing this chunkSize by the
/// Format chunk's wBlockAlign.
///
/// The Data Chunk is required. One, and only one, Data Chunk may appear
/// in a WAVE.

#include <algorithm>
#include <functional>
#include <string>
#include <stdexcept>
#include <cstdint>

#ifndef __SAWAVHEADER__
#define __SAWAVHEADER__

#define FormatID 'fmt '   
#define DataID 'data'  /* chunk ID for data Chunk */

namespace SimpleAudio
{
  /// \struct saOutputStream
  /// \brief A class for audio .wav file headers
  typedef struct saWavHeader
  {
    char     RiffChunkID[4];
    int32_t  RiffChunkSize;
    char     RiffFormat[4];
    char     FormatChunkID[4];
    int32_t  FormatChunkSize;
    int16_t  FormatTag;
    uint16_t FormatChannels;
    uint32_t FormatSamplesPerSec;
    uint32_t FormatAvgBytesPerSec;
    uint16_t FormatBlockAlign;
    uint16_t FormatBitsPerSample;
    char     DataChunkID[4];
    int32_t  DataChunkSize;
  } saWavHeader;

  void saSwap2Bytes(char *buf);
  void saSwap4Bytes(char *buf);
  void saSwapShort(uint16_t& buffer);
  void saSwapShort(int16_t& buffer);
  void saSwapShort(int16_t* buffer, std::size_t shorts);
  void saSwapLong(int32_t& buffer);
  void saSwapLong(uint32_t& buffer);
  bool saIsLittleEndian();
  bool saCompareID(char ID[4], std::string Str);
  void saSwapWavHeader(saWavHeader* wavHeader);

  template<typename T> 
  struct saSwapSample 
  {
    void operator() (T& x) 
    {
      (void)x;
      throw std::logic_error("can't swap sample size");
    }
  };
 
  template <>
  struct saSwapSample<int16_t> 
  {
    void operator() (int16_t& buffer) 
    {
      saSwap2Bytes(reinterpret_cast<char*>(&buffer));
    }
  };

  template <>
  struct saSwapSample<uint16_t>
  {
    void operator() (uint16_t& buffer) 
    {
      saSwap2Bytes(reinterpret_cast<char*>(&buffer));
    }
  };

  template <>
  struct saSwapSample<int32_t>
  {
    void operator() (int32_t& buffer) 
    {
      saSwap4Bytes(reinterpret_cast<char*>(&buffer));
    }
  };

  template <>
  struct saSwapSample<uint32_t> 
  {
    void operator() (uint32_t& buffer) 
    {
      saSwap4Bytes(reinterpret_cast<char*>(&buffer));
    }
  };

}

#endif
