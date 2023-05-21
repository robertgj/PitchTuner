/// \file ButterworthFilter_test.cc
//

// StdC++ include files
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

// Local include files
#include "saSample.h"
#include "saInputFileSelector.h"
#include "saInputSource.h"
#include "saInputFileWav.h"
#include "saInputStream.h"
#include "saOutputFileSelector.h"
#include "saOutputSink.h"
#include "saOutputStream.h"
#include "saOutputFileWav.h"
#include "ButterworthFilter.h"

static void Usage()
{
  std::cerr << "Allowed options:" 
            << std::endl;
  std::cerr << " --help                 produce help message" 
            << std::endl;
  std::cerr << " --high_pass_LN         use high pass low-noise order 2" 
            << std::endl;
  std::cerr << " --high_pass_GM         use high pass Gray-Markel order 3" 
            << std::endl;
  std::cerr << " --high_pass_LS         use high pass Stoyanov order 3" 
            << std::endl;
  std::cerr << " --seconds arg          seconds to read"
            << std::endl;
  std::cerr << " --cutoff arg           cutoff frequency"
            << std::endl;
  std::cerr << " --inFile arg           read from wav file"
            << std::endl;
  std::cerr << " --outFile arg          write to wav file"
            << std::endl;
}

void ParseOptions(int argc, 
                  char* argv[], 
                  bool& helpReq,
                  bool& high_pass_LN,
                  bool& high_pass_GM,
                  bool& high_pass_LS,
                  std::size_t& seconds, 
                  float& cutoff,
                  std::string& inFileName,
                  std::string& outFileName)
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

  opt = std::find(options.begin(), options.end(), "--high_pass_LN");
  if (opt != options.end())
    {
      high_pass_LN = true;
      high_pass_GM = false;
      high_pass_LS = false;
    }

  opt = std::find(options.begin(), options.end(), "--high_pass_GM");
  if (opt != options.end())
    {
      high_pass_LN = false;
      high_pass_GM = true;
      high_pass_LS = false;
    }

  opt = std::find(options.begin(), options.end(), "--high_pass_LS");
  if (opt != options.end())
    {
      high_pass_LN = false;
      high_pass_GM = false;
      high_pass_LS = true;
    }

  opt = std::find(options.begin(), options.end(), "--seconds");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> seconds;
    }

  opt = std::find(options.begin(), options.end(), "--cutoff");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> cutoff;
    }

  opt = std::find(options.begin(), options.end(), "--inFile");
  if (opt != options.end())
    {
      inFileName = *(++opt);
    }

  opt = std::find(options.begin(), options.end(), "--outFile");
  if (opt != options.end())
    {
      outFileName = *(++opt);
    }
}

using namespace SimpleAudio;

int main(int argc, char* argv[])
{
  try
    {
      // Default arguments
      bool helpReq = false;
      bool high_pass_LN = false;
      bool high_pass_GM = false;
      bool high_pass_LS = false;
      std::size_t seconds = 1;
      float cutoff = 1000.0;
      std::size_t sampleRate = 48000;
      std::size_t msDeviceLatency = 500;
      std::string inFileName("");
      std::string outFileName("");

      // Parse arguments
      ParseOptions (argc, argv, helpReq,
                    high_pass_LN, high_pass_GM, high_pass_LS,
                    seconds, cutoff, inFileName, outFileName);

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
      if (inFileName.size() == 0)
        {
          std::cerr << "No input file specified!" << std::endl ; 
          return -1;
        }
      std::string inName;
      inName.assign(inFileName.begin(), inFileName.end());
      std::unique_ptr<saInputStream> is( saInputStreamOpenFile(inName) );
      sampleRate = is->GetFramesPerSecond();

      // Open the output stream
      if (outFileName.size() == 0)
        {
          std::cerr << "No output file specified!" << std::endl ; 
          return -1;
        }
      std::string outName;
      outName.assign(outFileName.begin(), outFileName.end());    
      std::unique_ptr<saOutputStream> 
        os(saOutputStreamOpenFile(outName, sampleRate));

      // Define filters
      ButterworthHighPass2ndOrderFilter<saSample>
        hp2_LN(cutoff, sampleRate);
      updateFilter<saSample> updateHp2_LN(&hp2_LN);

      ButterworthHighPass3rdOrderFilter<saSample>
        hp3_GM(cutoff, sampleRate);
      updateFilter<saSample> updateHp3_GM(&hp3_GM);

      ButterworthHighPassStoyanov3rdOrderFilter<saSample>
        hp3_LS(cutoff, sampleRate);
      updateFilter<saSample> updateHp3_LS(&hp3_LS);

      ButterworthLowPass4thOrderFilter<saSample>
        lp(cutoff, sampleRate);
      updateFilter<saSample> updateLp(&lp);

      updateFilter<saSample> *update;
      if (high_pass_LN)
        {
          update = &updateHp2_LN;
          std::cerr << "High pass low-noise 2rd order filter, "
                    << "cutoff frequency " << cutoff 
                    << " ,sample rate " << sampleRate
                    << std::endl;
        }
      else if (high_pass_GM)
        {
          update = &updateHp3_GM;
          std::cerr << "High pass Gray and Markel 3nd order filter, "
                    << "cutoff frequency " << cutoff
                    << " ,sample rate " << sampleRate
                    << std::endl;
        }
      else if (high_pass_LS)
        {
          update = &updateHp3_LS;
          std::cerr << "High pass Stoyanov 3nd order filter, "
                    << "cutoff frequency " << cutoff
                    << ", sample rate " << sampleRate
                    << std::endl;
        }
      else
        {
          update = &updateLp;
          std::cerr << "Low pass 4th order filter, "
                    << "cutoff frequency " << cutoff
                    << ", sample rate " << sampleRate
                    << std::endl;
        }
          
      // Read
      std::size_t samplesToRead = seconds*sampleRate;
      std::size_t samplesWrittenSoFar = 0;
      const std::size_t bufSize = msDeviceLatency*sampleRate/1000;
      std::vector<saSample> buffer(bufSize);
      // Note that transform unary op must not alter its argument
      std::vector<saSample> output(bufSize);

      while( !is->EndOfSource() && (samplesWrittenSoFar < samplesToRead) )
        {
          buffer.resize(0);
          std::size_t samplesRead = is->Read(buffer, bufSize);

          std::size_t samplesToWrite = 
            samplesWrittenSoFar+samplesRead > samplesToRead ?
            samplesToRead-samplesWrittenSoFar : samplesRead;

          // Run the filter
          output.resize(0);
          std::transform(buffer.begin(), 
                         buffer.end(), 
                         std::back_inserter(output), 
                         *update);
          
          std::size_t samplesWritten = os->Write(output, samplesToWrite);
          samplesWrittenSoFar += samplesWritten;
        }

      // Done
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl ; 
      return -1;
    }

  return 0;
}

