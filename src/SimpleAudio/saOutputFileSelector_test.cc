// \file saOutputFileSelector_test.cc
//

// StdC++ include files
#include <iostream>
#include <iomanip>
#include <memory>

// Local include files
#include "saOutputFileSelector.h"
#include "saOutputFileWav.h"

using namespace SimpleAudio;
int main(void)
{
  // Register file handler
  const bool wavRegistered = saSingletonOutputFileSelector::Instance().
    RegisterFileType(".wav", saOutputSinkOpenFileWav);
  if (!wavRegistered)
    {
      std::cerr << "wav not registered!" << std::endl;
    }

  // Open the input stream
  std::unique_ptr<saOutputSink> 
    os(saSingletonOutputFileSelector::Instance().OpenFile("test.wav", 10000, 1));

  // Write some frames
  for (std::size_t i = 0; i < 20; i++)
    {
      saOutputSink::saOutputFragment fragment = os->Claim();
      for (std::size_t j = 0;
           j < os->GetSamplesPerFrame()*os->GetFramesPerFragment(); 
           j++)
        {
          fragment[j] = (SimpleAudio::saOutputSink::saSinkType)j;
        }
      os->Dispatch(fragment, os->GetFramesPerFragment());
    }

  // Done. Assume destructor closes file
  return 0;
}
