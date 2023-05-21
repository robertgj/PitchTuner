// \file saInputStreamFile_test.cc
//

// StdC++ include files
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <iterator>

// Local include files
#include "saInputFileSelector.h"
#include "saInputFileWav.h"
#include "saInputFileAscii.h"
#include "saInputStream.h"

using namespace SimpleAudio;

static void Usage()
{
  std::cerr << "Allowed options:\n";
  std::cerr << "  --help                produce help message\n";
  std::cerr << "  --read_test           test reading frames\n";
  std::cerr << "  --clear_test          test clear function\n";
  std::cerr << "  --eos_test            test end-of-source function\n";
  std::cerr << "  --frames_read_test    test number of frames read function\n";
  std::cerr << "  --frames arg          frames to read from source\n";
  std::cerr << "  --channel arg         channel to read from frame\n";
  std::cerr << "  --file arg            read from file\n";
}
static void ParseOptions(int argc, 
                         char* argv[], 
                         std::string& sourceName,
                         std::size_t& readSize, 
                         bool& readSize_set,
                         std::size_t& channel,
                         bool& channel_set,
                         bool& helpReq,
                         bool& read_test,
                         bool& clear_test,
                         bool& eos_test,
                         bool& frames_read_test)
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

  helpReq = false;
  opt = std::find(options.begin(), options.end(), "--help");
  if (opt != options.end())
    {
      Usage();
      helpReq = true;
      return;
    }

  // Test memory leak checking
  opt = std::find(options.begin(), options.end(), "--leak_test");
  if (opt != options.end())
    {
#ifdef __SA_LEAK_DEBUG__
      // Test debug memory leak detection:
      std::cerr << "Testing memory leak detection" << "\n";
      volatile int* tmp = new int(1);
      *tmp = 2;
#endif
    } 

  read_test = false;
  opt = std::find(options.begin(), options.end(), "--read_test");
  if (opt != options.end())
    {
      std::cerr << "Testing read" << "\n";
      read_test = true;
    } 

  clear_test = false;
  opt = std::find(options.begin(), options.end(), "--clear_test");
  if (opt != options.end())
    {
      std::cerr << "Testing clear function" << "\n";
      clear_test = true;
    } 

  eos_test = false;
  opt = std::find(options.begin(), options.end(), "--eos_test");
  if (opt != options.end())
    {
      std::cerr << "Testing end-of-source function" << "\n";
      eos_test = true;
    } 

  // Test number of frames read function
  frames_read_test = false;
  opt = std::find(options.begin(), options.end(), "--frames_read_test");
  if (opt != options.end())
    {
      std::cerr << "Testing number of frames read function" << "\n";
      frames_read_test = true;
    } 

  // Number of frames to read
  readSize = 0;
  readSize_set = false;
  opt = std::find(options.begin(), options.end(), "--frames");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> readSize;
      readSize_set = true;
    } 

  // Channel to read
  channel = 0;
  channel_set = false;
  opt = std::find(options.begin(), options.end(), "--channel");
  if (opt != options.end())
    {
      std::istringstream sstr(*(++opt));
      sstr >> channel;
      channel_set = true;
    } 

  // Read from device or file
  sourceName = "";
  opt = std::find(options.begin(), options.end(), "--file");
  if (opt != options.end()) 
    {
      sourceName = *(++opt);
    } 
}

int main(int argc, char* argv[])
{
  try
    {
      // Default arguments
      std::string fileName("");
      std::size_t readSize = 0;
      bool readSize_set = false;
      std::size_t channel = 0;
      bool channel_set = false;
      bool helpReq = false;
      bool read_test = false;
      bool clear_test = false;
      bool eos_test = false;
      bool frames_read_test = false;

      // Parse arguments
      ParseOptions(argc, argv, fileName, 
                   readSize, readSize_set, channel, channel_set,
                   helpReq, read_test, clear_test, eos_test, frames_read_test);

      if (helpReq)
        {
          return 0;
        }

      // Sanity check
      if (fileName.length() == 0)
        {
          std::cerr << "No input file specified" << "\n";
          return -1;
        }

      // Register file handlers
      const bool wavRegistered = saSingletonInputFileSelector::Instance().
        RegisterFileType(".wav", saInputSourceOpenFileWav);
      if (!wavRegistered)
        {
          std::cerr << "failed to register wav!" << "\n";
          return -1;
        }
      const bool ascRegistered = saSingletonInputFileSelector::Instance().
        RegisterFileType(".asc", saInputSourceOpenFileAscii);
      if (!ascRegistered)
        {
          std::cerr << "failed to register asc!" << "\n";
          return -1;
        }

      // Useful conversion functor
      saSampleToSink<saInputSource::saSourceType> conv;

      // Open the input stream
      std::string name(fileName.begin(), fileName.end());
      std::unique_ptr<saInputStream> is(saInputStreamOpenFile(name.c_str()));
      if ( (readSize_set == true) && (readSize > is->GetFramesPerStream()) )
        {
          readSize = is->GetFramesPerStream();
        }

      // Echo
      std::cerr << "Reading " << readSize 
                << " frames from " << fileName;
      if (channel_set)
        {
          std::cerr << " at channel " << channel;
        }
      std::cerr << "\n";

      // Status
      std::cerr << "Frames per sec. " 
                << is->GetFramesPerSecond() << "\n" ;
      std::cerr << "Samples per frame " 
                << is->GetSamplesPerFrame() << "\n" ;
      std::cerr << "Frames per fragment " 
                << is->GetFramesPerFragment() << "\n" ;
      std::cerr << "Frames per stream " 
                << is->GetFramesPerStream() << "\n" ;

      // Read some samples
      if (read_test)
        {
          std::cerr << "Test read" << "\n";

          std::size_t framesSoFar=0; 
          std::vector< saSample > buf;
          while(framesSoFar<readSize)
            {
              std::size_t thisRead = readSize-framesSoFar < readSize ? 
                readSize-framesSoFar : readSize;
              buf.clear();
              framesSoFar += 
                channel_set ? 
                is->Read(buf, thisRead, channel) :
                is->Read(buf, thisRead);
              std::transform(buf.begin(), 
                             buf.end(), 
                             std::ostream_iterator<saInputSource::saSourceType>
                             (std::cout,"\n"), 
                             conv);
            }
        }

      // Test clear buffer
      if( clear_test )
        {
          std::cerr << "Test clear buffer" << "\n";

          std::size_t framesSoFar; 
          std::vector< saSample > buf;
          framesSoFar = is->Read(buf, readSize);
          std::cerr << conv(buf[0]) << "\n";
          std::cerr << conv(buf[1]) << "\n";
          std::cerr << "Frames read from stream " 
                    << is->GetFramesReadFromStream() 
		    << " : " << framesSoFar << "\n" ;
          is->Clear();
          buf.clear();
          framesSoFar = is->Read(buf, is->GetFramesPerFragment());
          std::cerr << conv(buf[0]) << "\n";
          std::cerr << conv(buf[1]) << "\n";
          std::cerr << conv(buf[is->GetFramesPerFragment()-1]) << "\n";
          std::cerr << "Frames read from stream " 
                    << is->GetFramesReadFromStream()
		    << " : " << framesSoFar << "\n" ;
        }
     
      // Test end-of-source
      if( eos_test )
        {
          std::cerr << "Test end-of-source" << "\n";

          std::size_t framesSoFar; 
          std::vector< saSample > buf;
          framesSoFar = is->Read(buf, 1); 
          std::cout << "Read " << conv(buf[0]) << "\n";
          std::cout << "Frames so far "  << framesSoFar << "\n";
          std::cout << "EndOfSource() " << is->EndOfSource() << "\n";          
          framesSoFar += is->Read(buf, is->GetFramesPerStream() - 2);
          std::cout << "Frames so far "  << framesSoFar << "\n";
          std::cout << "EndOfSource() "  << is->EndOfSource() << "\n";          
          framesSoFar += is->Read(buf, 1);
          std::cout << "Frames so far "  << framesSoFar << "\n";
          std::cout << "EndOfSource() "  << is->EndOfSource() << "\n";          
          std::cout << "Frames read from stream " 
                    << is->GetFramesReadFromStream()
		    << " : " << framesSoFar << "\n" ;
        }
      
      // Test frames read
      if ( frames_read_test )
        {
          std::cerr << "Test frames read" << "\n";

          std::size_t framesSoFar; 
          std::vector< saSample > buf;
          framesSoFar = is->Read(buf, readSize);
          std::cerr << conv(buf[0]) << "\n";
          std::cerr << conv(buf[1]) << "\n";
          std::cerr << "Frames read from stream " 
                    << is->GetFramesReadFromStream()
		    << " : " << framesSoFar << "\n" ;

          buf.clear();
          framesSoFar = is->Read(buf, 1);
          std::cerr << conv(buf[0]) << "\n";
          std::cerr << "Frames read from stream " 
                    << is->GetFramesReadFromStream() << "\n" ;
          framesSoFar = is->Read(buf, is->GetFramesPerSecond());
          std::cerr << conv(buf[0]) << "\n";
          std::cerr << conv(buf[is->GetFramesPerSecond()]) << "\n";
          std::cerr << "Frames read from stream " 
                    << is->GetFramesReadFromStream()
		    << " : " << framesSoFar << "\n" ;
        }

      // Done. Assume destructor closes stream
    }
  catch(std::exception& excpt)
    {
      std::cerr << excpt.what() << "\n" ; 
      return -1;
    }

  return 0;
}
