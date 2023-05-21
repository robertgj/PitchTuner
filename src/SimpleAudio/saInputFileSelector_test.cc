// \file saInputFileSelector_test.cc
//

// StdC++ include files
#include <iostream>
#include <iomanip>
#include <memory>

// Local include files
#include "saInputFileSelector.h"
#include "saInputFileWav.h"

using namespace SimpleAudio;
int main(void)
{
  // Register file handler
  const bool wavRegistered = saSingletonInputFileSelector::Instance().
    RegisterFileType(".wav", saInputSourceOpenFileWav);
  if (!wavRegistered)
    {
      std::cerr << "wav not registered!" << std::endl;
    }

  // Open the input stream
  std::unique_ptr<saInputSource> 
    is(saSingletonInputFileSelector::Instance().OpenFile("test.wav"));

  // Status
  std::cerr << "Frames per sec. " 
             << is->GetFramesPerSecond() << std::endl ;
  std::cerr << "Samples per frame " 
             << is->GetSamplesPerFrame() << std::endl ;
  std::cerr << "Frames per fragment " 
             << is->GetFramesPerFragment() << std::endl ;
  std::cerr << "Frames per source " 
             << is->GetFramesPerSource() << std::endl ;
  std::cerr << "device latency(ms) " 
             << is->GetMsDeviceLatency() << std::endl ;


  // Read some frames
  do
    {
      std::size_t framesRead;
      saInputSource::saInputFragment fragment = is->Collect(framesRead);
      std::size_t samplesPerFrame = is->GetSamplesPerFrame();
      for (size_t frameNum=0; frameNum<framesRead; frameNum++)
        {
          std::cout << fragment[frameNum*samplesPerFrame] << std::endl;
        }
      is->Release(fragment);
    }
  while (!is->EndOfSource());

  // Done. Assume destructor closes file
  return 0;
}
