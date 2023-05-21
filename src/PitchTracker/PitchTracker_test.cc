// PitchTracker_test.cc

// StdC++ include files
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <list>
#include <iterator>

// Local include files
#include "saSample.h"
#include "saInputSource.h"
#include "saInputFileWav.h"
#include "saInputStream.h"
#include "PreProcessor.h"
#include "PitchTracker.h"

// Options:
//   --help                        list these options
//   --debug                       enable debugging output
//   --maxPitchSamples int         pitch samples to read
//   --channel int                 channel from frame
//   --sampleRate int              sample rate in Hz
//   --msDeviceLatency int         device buffer storage in ms
//   --msWindow int                window width in ms
//   --msTmax int                  correlation width (lags) in ms
//   --msTsample int               pitch sample interval in ms
//   --inputLpFilterCutoff int     pre-filter low-pass in Hz
//   --baseLineHpFilterCutoff int  pre-filter high-pass in Hz
//   --disableHpFilter bool        disable the high-pass filter
//   --disableAgc      bool        disable Automatic Gain Control
//   --subSampledInput int         sub-sampling ratio on the input
//   --threshold float             cumulative difference threshold
//   --removeDC bool               remove slowly varying DC
//   --file name
//   --device name
//
void Usage()
{
  std::cerr << "Allowed options:" 
            << std::endl;
  std::cerr << "  --help                        produce help message" 
            << std::endl;
  std::cerr << "  --debug                       debug messages" 
            << std::endl;
  std::cerr << "  --maxPitchSamples arg         number of pitch samples to read" 
            << std::endl;
  std::cerr << "  --channel arg                 channel from frame" 
            << std::endl;
  std::cerr << "  --sampleRate arg              device sample rate" 
            << std::endl;
  std::cerr << "  --msDeviceLatency arg         device buffer storage in ms" 
            << std::endl;
  std::cerr << "  --msWindow arg                window length in ms" 
            << std::endl;
  std::cerr << "  --msTmax arg                  autocorrelation length in ms" 
            << std::endl;
  std::cerr << "  --msTsample arg               pitch sample interval in ms" 
            << std::endl;
  std::cerr << "  --subSample arg               sub-sampling ratio" 
            << std::endl;
  std::cerr << "  --inputLpFilterCutoff    arg  pre-filter low-pass in Hz" 
            << std::endl;
  std::cerr << "  --baseLineHpFilterCutoff arg  pre-filter high-pass in Hz" 
            << std::endl;
  std::cerr << "  --disableHpFilter bool        disable the high-pass filter" 
            << std::endl;
  std::cerr << "  --disableAgc bool             disable AGC" 
            << std::endl;
  std::cerr << "  --threshold arg               cumulative difference threshold" 
            << std::endl ;
  std::cerr << "  --removeDC bool               remove slowly varying DC" 
            << std::endl ;
  std::cerr << "  --file arg                    read from file" 
            << std::endl;
  std::cerr << "  --device arg                  read from device" 
            << std::endl;
  std::cerr << std::endl;
}

void ParseOptions(int argc, 
                  char* argv[], 
                  bool& helpReq,
                  bool& debugReq,
                  std::size_t& maxPitchSamples, 
                  std::size_t& channel,
                  std::size_t& sampleRate,
                  std::size_t& msDeviceLatency,
                  std::size_t& msWindow,
                  std::size_t& msTmax,
                  std::size_t& msTsample,
                  std::size_t& subSample,
                  float& inputLpFilterCutoff,
                  float& baseLineHpFilterCutoff,
                  bool& disableHpFilter,
                  bool& disableAgc,
                  float& threshold,
                  bool& removeDC,
                  std::string& sourceName,
                  bool& fileReq,
                  bool& maxPitchSamplesSet)
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
    }

  opt = std::find(options.begin(), options.end(), "--debug");
  if (opt != options.end())
    {
      debugReq = true;
    }

  opt = std::find(options.begin(), options.end(), "--maxPitchSamples");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt)); 
      sstr >> maxPitchSamples;
      maxPitchSamplesSet = true;
    }
  
  opt = std::find(options.begin(), options.end(), "--channel");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> channel;
    } 
  
  opt = std::find(options.begin(), options.end(), "--sampleRate");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> sampleRate;
    }

  opt = std::find(options.begin(), options.end(), "--msDeviceLatency");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> msDeviceLatency;
    }

  opt = std::find(options.begin(), options.end(), "--msWindow");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> msWindow;
    } 

  opt = std::find(options.begin(), options.end(), "--msTmax");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> msTmax;
    } 

  opt = std::find(options.begin(), options.end(), "--msTsample");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> msTsample;
    } 

  opt = std::find(options.begin(), options.end(), "--subSample");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> subSample;
    } 

  opt = std::find(options.begin(), options.end(), "--inputLpFilterCutoff");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> inputLpFilterCutoff;
    } 

  opt = std::find(options.begin(), options.end(), "--baseLineHpfilterCutoff");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> baseLineHpFilterCutoff;
    } 

  opt = std::find(options.begin(), options.end(), "--disableHpFilter");
  if (opt != options.end())
    {
      disableHpFilter = true;
    } 

  opt = std::find(options.begin(), options.end(), "--disableAgc");
  if (opt != options.end())
    {
      disableAgc = true;
    } 

  opt = std::find(options.begin(), options.end(), "--threshold");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> threshold;
    } 

  opt = std::find(options.begin(), options.end(), "--removeDC");
  if (opt != options.end())
    {
      removeDC = true;
    } 

  opt = std::find(options.begin(), options.end(), "--file");
  if (opt != options.end())
    {
      sourceName = *(++opt);
      fileReq = true;
    }

  opt = std::find(options.begin(), options.end(), "--device");
  if (opt != options.end())
    {
      sourceName = *(++opt);
      fileReq = false;
    }
}

using namespace SimpleAudio;

int main(int argc, char* argv[])
{
  try
    {
      // Register file handlers
      const bool wavRegistered = 
        saSingletonInputFileSelector::Instance().
        RegisterFileType(".wav", saInputSourceOpenFileWav);
      if (!wavRegistered)
        {
          std::cerr << "register wav failed!" << std::endl;
          return -1;
        }

      // Default arguments
      bool helpReq = false;
      bool debugReq = false;
      std::size_t maxPitchSamples = 0;
      std::size_t channel = 0;
      std::size_t sampleRate = 10000;
      std::size_t msDeviceLatency = 100;
      std::size_t msWindow = 25;
      std::size_t msTmax = 20;
      std::size_t msTsample = 10;
      std::size_t subSample = 1;
      float inputLpFilterCutoff = 1000;
      float baseLineHpFilterCutoff = 200;
      bool disableHpFilter = false;
      bool disableAgc = false;
      bool removeDC = false;
      float threshold = 0.1f;
      std::string sourceName("default");
      bool fileReq = false;
      bool maxPitchSamplesSet = false;

      // Parse arguments
      ParseOptions(argc, argv, helpReq, debugReq,
                   maxPitchSamples, channel, sampleRate,
                   msDeviceLatency, msWindow, msTmax, msTsample, subSample, 
                   inputLpFilterCutoff, baseLineHpFilterCutoff,
                   disableHpFilter, disableAgc,
                   threshold, removeDC, sourceName, fileReq, maxPitchSamplesSet);

      // Echo PitchTracker arguments
      if (debugReq)
        {
          std::cerr << "debug= "            << debugReq        << std::endl; 
          std::cerr << "maxPitchSamples= "  << maxPitchSamples << std::endl;
          std::cerr << "channel= "          << channel         << std::endl;
          std::cerr << "sampleRate= "       << sampleRate      << std::endl;
          std::cerr << "msDeviceLatency= "  << msDeviceLatency << std::endl;
          std::cerr << "msWindow= "         << msWindow        << std::endl;
          std::cerr << "msTmax= "           << msTmax          << std::endl;
          std::cerr << "msTsample= "        << msTsample       << std::endl;
          std::cerr << "subSample= "        << subSample       << std::endl;
          std::cerr << "inputLpFilterCutoff= "
                                        << inputLpFilterCutoff << std::endl; 
          std::cerr << "baseLineHpFilterCutoff= "
                                     << baseLineHpFilterCutoff << std::endl; 
          std::cerr << "disableHpFilter= "  << disableHpFilter << std::endl; 
          std::cerr << "disableAgcFilter= " << disableAgc      << std::endl; 
          std::cerr << "removeDC= "         << removeDC        << std::endl; 
          std::cerr << "threshold= "        << threshold       << std::endl; 
          std::cerr << "sourceName= "       << sourceName      << std::endl; 
        }
      
      // Check for help request
      if (helpReq)
        {
          return 0;
        }

      // Open the input stream
      std::string name(sourceName.begin(), sourceName.end());
      std::unique_ptr<saInputStream> 
        is( fileReq == true ? saInputStreamOpenFile(name) :
            saInputStreamOpenDevice
            (name, sampleRate, msDeviceLatency, 1, debugReq) );
      if ( fileReq )
        {
          sampleRate = is->GetFramesPerSecond();
          
          if ( !maxPitchSamplesSet )
            {
              maxPitchSamples = is->GetFramesPerStream();
              maxPitchSamples /= (msTsample*sampleRate/1000);
            }
        }

      // Create PreProcessor
      std::unique_ptr<PreProcessor> 
        pre(PreProcessorCreate(is.get(), channel, 
                               msWindow, msTmax, msTsample, subSample,
                               inputLpFilterCutoff, baseLineHpFilterCutoff,
                               disableHpFilter, disableAgc, debugReq));

      // Create PitchTracker
      std::unique_ptr<PitchTracker> 
        pt( PitchTrackerCreate
            ( pre->SubSampleRate(), pre->WindowLength(), pre->MaxLags(),
              threshold, removeDC, debugReq ) );
      
      std::vector<float> pitchList;
      std::size_t samplesSoFar = 0;
      while( samplesSoFar++ < maxPitchSamples && !is->EndOfSource() )
        {
          float pitch = pt->EstimatePitch( pre->Read() );
          pitchList.push_back(pitch);
        }

      // Echo pitch results
      std::cout.precision(4);
      std::copy(pitchList.begin(), pitchList.end(), 
                std::ostream_iterator<float>(std::cout, "\n"));

      // Done. Assume destructor closes stream
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << std::endl ; 
      return -1;
    }
  
  return 0;
}
