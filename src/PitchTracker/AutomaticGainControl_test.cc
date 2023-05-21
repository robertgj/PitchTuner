// AutomaticGainControl_test.cc
//
// A test function:
//  Fs=48000
//  f=600
//  a=[ones(1,Fs/2) zeros(1,Fs/2)];
//  aa=[a a a a]*2+0.1;
//  s=sin(2*pi*f*(0:(length(aa)-1))/Fs).*aa;
//  audiowrite("s600agc.wav", s', Fs);

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

#include "AutomaticGainControl.h"
#include "ButterworthFilter.h"

static void Usage()
{
  std::cerr << "Allowed options:" 
            << std::endl;
  std::cerr << " --help                  produce help message" 
            << std::endl;                
  std::cerr << " --debug                 produce debug output" 
            << std::endl;                
  std::cerr << " --seconds arg           seconds to read"
            << std::endl;                
  std::cerr << " --inFile arg            read from wav file"
            << std::endl;                
  std::cerr << " --outFile arg           write to wav file"
            << std::endl;                
  std::cerr << " --lpFilterCutoff        low pass filter cutoff in Hz"
            << std::endl;                
  std::cerr << " --hpFilterCutoff        high pass filter cutoff in Hz"
            << std::endl;                
  std::cerr << " --fastTauMs             fast time constant in ms"
            << std::endl;                
  std::cerr << " --slowTauMs             slow time constant in ms"
            <<  std::endl;
  std::cerr << " --peakTauMs             peak time constant in ms"
            << std::endl;
  std::cerr << " --lowerPeakThreshold    lower peak threshold"
            <<  std::endl;
  std::cerr << " --upperPeakThreshold    upper peak threshold"
            <<  std::endl;
  std::cerr << " --gainMin               maximum gain"
            <<  std::endl;
  std::cerr << " --gainMax               minimum gain"
            <<  std::endl;
}

void ParseOptions(int argc, 
                  char* argv[], 
                  bool& helpReq,
                  bool& debugReq,
                  std::size_t& seconds, 
                  std::string& inFileName,
                  std::string& outFileName,
                  float& lpFilterCutoff,
                  float& hpFilterCutoff,
                  std::size_t& fastTauMs,
                  std::size_t& slowTauMs,
                  std::size_t& peakTauMs,
                  float& lowerPeakThreshold,
                  float& upperPeakThreshold,
                  float& gainMin,
                  float& gainMax)
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

  opt = std::find(options.begin(), options.end(), "--debug");
  if (opt != options.end())
    {
      debugReq = true;
    }

  opt = std::find(options.begin(), options.end(), "--seconds");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> seconds;
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

  opt = std::find(options.begin(), options.end(), "--lpFilterCutoff");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> lpFilterCutoff;
    }

  opt = std::find(options.begin(), options.end(), "--hpFilterCutoff");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> hpFilterCutoff;
    }

  opt = std::find(options.begin(), options.end(), "--fastTauMs");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> fastTauMs;
    }

  opt = std::find(options.begin(), options.end(), "--slowTauMs");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> slowTauMs;
    }

  opt = std::find(options.begin(), options.end(), "--peakTauMs");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> peakTauMs;
    }

  opt = std::find(options.begin(), options.end(), "--lowerPeakThreshold");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> lowerPeakThreshold;
    }

  opt = std::find(options.begin(), options.end(), "--upperPeakThreshold");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> upperPeakThreshold;
    }

  opt = std::find(options.begin(), options.end(), "--gainMin");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> gainMin;
    }

  opt = std::find(options.begin(), options.end(), "--gainMax");
  if (opt != options.end())
    {
      std::istringstream(*(++opt)) >> gainMax;
    }
}

using namespace SimpleAudio;

int main(int argc, char* argv[])
{
  try
    {
      // Default arguments
      std::size_t msDeviceLatency = 500;
      bool helpReq = false;
      bool debugReq = false;
      std::size_t seconds = 1;
      std::string inFileName("");
      std::string outFileName("");
      std::size_t sampleRate = 48000;
      float lpFilterCutoff = 1000.0f;
      float hpFilterCutoff = 50.0f;
      std::size_t fastTauMs = 50;
      std::size_t slowTauMs = 50;
      std::size_t peakTauMs = 50;
      float lowerPeakThreshold = 0.4f;
      float upperPeakThreshold = 0.6f;
      float gainMin = 0.1f;
      float gainMax = 10.0f;

      // Parse arguments
      ParseOptions (argc, argv, helpReq, debugReq,
                    seconds, inFileName, outFileName,
                    lpFilterCutoff, hpFilterCutoff, 
                    fastTauMs, slowTauMs, peakTauMs,
                    lowerPeakThreshold, upperPeakThreshold,
                    gainMin, gainMax);

      // Check for help request
      if (helpReq)
        {
          return 0;
        }

      // Define low pass filter
      ButterworthLowPass4thOrderFilter<saSample> 
        lp(lpFilterCutoff, sampleRate);

      // Define high pass filter
      ButterworthHighPass3rdOrderFilter<saSample> 
        hp(hpFilterCutoff, sampleRate);

      // Define AGC
      AutomaticGainControl<saSample> agc
        (sampleRate, fastTauMs, slowTauMs, peakTauMs,
         lowerPeakThreshold, upperPeakThreshold, gainMin, gainMax, 
         debugReq);

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

      // Report 
      std::cerr << seconds << " seconds of data "
                << " read from " << inFileName << " to " << outFileName 
                << std::endl;
      std::cerr << "At sample rate " << sampleRate 
                << std::endl;
      std::cerr << " With low pass filter cutoff " << lpFilterCutoff
                << " and high pass filter cutoff " << hpFilterCutoff
                << std::endl;
      std::cerr << "Fast tau(ms) " << fastTauMs
                << " , slow tau(ms) " << slowTauMs
                << " , peak tau(ms) " << peakTauMs
                << std::endl;
      std::cerr << "Lower peak threshold " << lowerPeakThreshold
                << " , upper peak threshold " << upperPeakThreshold
                << std::endl;
     std:: cerr << "Minimum gain " << gainMin
                << " , maximum gain " << gainMax
                << std::endl;

      // Read
      std::size_t samplesToRead = seconds*sampleRate;
      std::size_t samplesWrittenSoFar = 0;
      const std::size_t bufSize = msDeviceLatency*sampleRate/1000;
      std::vector<saSample> buffer(bufSize);
      std::vector<saSample> outputLp(bufSize);
      std::vector<saSample> outputHp(bufSize);
      std::vector<saSample> outputAgc(bufSize);

      while( !is->EndOfSource() && (samplesWrittenSoFar < samplesToRead) )
        {
          buffer.resize(0);
          std::size_t samplesRead = is->Read(buffer, bufSize);

          std::size_t samplesToWrite = 
            samplesWrittenSoFar+samplesRead > samplesToRead ?
            samplesToRead-samplesWrittenSoFar : samplesRead;

          outputLp.resize(0);
          std::transform(buffer.begin(), 
                         buffer.end(), 
                         std::back_inserter(outputLp),
                         updateFilter<saSample>(&lp));

          outputHp.resize(0);
          std::transform(outputLp.begin(), 
                         outputLp.end(), 
                         std::back_inserter(outputHp), 
                         updateFilter<saSample>(&hp));

          outputAgc.resize(0);
          std::transform(outputHp.begin(), 
                         outputHp.end(), 
                         std::back_inserter(outputAgc), 
                         updateFilter<saSample>(&agc));
          
          std::size_t samplesWritten = os->Write(outputAgc, samplesToWrite);
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
