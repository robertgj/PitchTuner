// saOutputStreamFile_test.cc

#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "saSample.h"
#include "saInputFileSelector.h"
#include "saInputSource.h"
#include "saInputFileWav.h"
#include "saInputFileAscii.h"
#include "saInputStream.h"
#include "saOutputFileSelector.h"
#include "saOutputSink.h"
#include "saOutputStream.h"
#include "saOutputFileWav.h"

static void Usage()
{
  std::cerr << "Allowed options:\n"; 
  std::cerr << "  --help                  produce help message\n"; 
  std::cerr << "  --seconds arg           seconds to read\n";
  std::cerr << "  --frameRate arg         device frames per second\n";
  std::cerr << "  --latencyMs arg         ms per buffer\n";
  std::cerr << "  --inDevice arg          read from device\n";
  std::cerr << "  --inFile arg            read from wav file\n";
  std::cerr << "  --outDevice arg         write to device\n";
  std::cerr << "  --outFile arg           write to wav file\n";
}

void ParseOptions(int argc, 
                  char* argv[], 
                  bool& helpReq,
                  std::size_t& seconds, 
                  std::size_t& framesPerSecond,
                  std::size_t& latencyMs,
                  std::string& inFileName,
                  std::string& inDeviceName,
                  std::string& outFileName,
                  std::string& outDeviceName)
{
  if (argc < 1)
    {
      Usage();
      return;
    }

  // Define a vector of options
  std::vector<std::string> options(&(argv[1]), &(argv[argc]));

  // Search for options
  std::vector<std::string>::iterator opt;

  opt = std::find(options.begin(), options.end(), "--help");
  if (opt != options.end())
    {
      Usage();
      helpReq = true;
      return;
    }

  opt = std::find(options.begin(), options.end(), "--seconds");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> seconds;
    }

  opt = std::find(options.begin(), options.end(), "--frameRate");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> framesPerSecond;
    }

  opt = std::find(options.begin(), options.end(), "--latencyMs");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> latencyMs;
    }

  opt = std::find(options.begin(), options.end(), "--inFile");
  if (opt != options.end())
    {
      inFileName = *(++opt);
    }

  opt = std::find(options.begin(), options.end(), "--inDevice");
  if (opt != options.end())
    {
      inDeviceName = *(++opt);
    }

  opt = std::find(options.begin(), options.end(), "--outFile");
  if (opt != options.end())
    {
      outFileName = *(++opt);
    }

  opt = std::find(options.begin(), options.end(), "--outDevice");
  if (opt != options.end())
    {
      outDeviceName = *(++opt);
    }
}

using namespace SimpleAudio;

int main(int argc, char* argv[])
{
  try
    {
      // Default arguments
      bool helpReq = false;
      std::size_t seconds = 1;
      std::size_t framesPerSecond = 10000;
      std::size_t latencyMs = 500;
      std::string inFileName("");
      std::string inDeviceName("");
      std::string outFileName("");
      std::string outDeviceName("");

      // Parse arguments
      ParseOptions (argc, argv, helpReq,
                    seconds, framesPerSecond, latencyMs, 
                    inFileName, inDeviceName, outFileName, outDeviceName);

      // Check for help request
      if (helpReq)
        {
          return 0;
        }

      // Register file handlers
      const bool wavInRegistered = saSingletonInputFileSelector::Instance().
        RegisterFileType(".wav", saInputSourceOpenFileWav);
      if (!wavInRegistered)
        {
          std::cerr << "failed to register wav for input!" << std::endl;
          return -1;
        }
      const bool wavOutRegistered = saSingletonOutputFileSelector::Instance().
        RegisterFileType(".wav", saOutputSinkOpenFileWav);
      if (!wavOutRegistered)
        {
          std::cerr << "failed to register wav for output!" << std::endl;
          return -1;
        }

      // Open the input stream
      std::string inName;
      if ( (inDeviceName.size() == 0) && (inFileName.size() == 0))
        {
          std::cerr << "No input device or file specified!" << std::endl ; 
          return -1;
        }
      else if ((inDeviceName.size() != 0) && (inFileName.size() == 0))
        {
          inName = inDeviceName;
        }
      else
        {
          inName = inFileName;
        }
      std::unique_ptr<saInputStream>
        is( inDeviceName.size() != 0 ?
            saInputStreamOpenDevice(inName, framesPerSecond, latencyMs) :
            saInputStreamOpenFile(inName) );
      framesPerSecond = is->GetFramesPerSecond();

      // Open the output stream
      std::string outName;
      if ( (outDeviceName.size() == 0) && (outFileName.size() == 0))
        {
          std::cerr << "No output device or file specified!" << std::endl ; 
          return -1;
        }
      else if ((outDeviceName.size() != 0) && (outFileName.size() == 0))
        {
          outName = outDeviceName;
        }
      else
        {
          outName = outFileName;
        }
      
      std::size_t samplesPerFrame = is->GetSamplesPerFrame();
      std::unique_ptr<saOutputStream> 
        os( outDeviceName.size() != 0 ?
            saOutputStreamOpenDevice
            (outName, framesPerSecond, latencyMs, samplesPerFrame ) :
            saOutputStreamOpenFile(outName, framesPerSecond, samplesPerFrame) );

      // Read
      std::size_t framesToRead = seconds*framesPerSecond;
      std::size_t framesPerRead = (latencyMs*framesPerSecond)/1000;
      std::size_t framesReadSoFar = 0;
      std::size_t framesWrittenSoFar = 0;
      std::vector<saSample> buffer(samplesPerFrame*framesToRead);
      while( !is->EndOfSource() && (framesReadSoFar < framesToRead) ) 
        {
          buffer.resize(0);
          std::size_t framesReadThisRead = is->Read(buffer, framesPerRead);

          std::size_t framesToWriteThisWrite = 
            framesReadSoFar+framesReadThisRead > framesToRead ?
            framesToRead-framesReadSoFar : framesReadThisRead;

          std::size_t framesWrittenThisWrite = 
            os->Write(buffer, framesToWriteThisWrite);

          framesReadSoFar += framesReadThisRead;
          framesWrittenSoFar += framesWrittenThisWrite;          
        }

      // Done.
      (void)framesReadSoFar;
      (void)framesWrittenSoFar;
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl ; 
      return -1;
    }

  return 0;
}



