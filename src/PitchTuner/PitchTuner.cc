/// \file PitchTuner.cc
/// \brief wxWidgets application for the PitchTracker pitch estimator

// StdC++ headers
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <ctime>
#include <cstdlib>

// wxWidgets include files
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <wx/thread.h>
#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/toolbar.h>
#include <wx/gdicmn.h>
#include <wx/display.h>
#include <wx/window.h>
#include <wx/event.h>

// Local include files
#include "saDeviceInfo.h"
#include "saInputStream.h"
#include "saInputFileSelector.h"
#include "saInputFileWav.h"
#include "PreProcessor.h"
#include "PitchTracker.h"
#include "PitchTuner.h"
#include "PitchTunerFrame.h"

/// \struct optionsDebug_t
/// \brief PitchTuner options 
typedef struct optionsPitchTuner_t {
  /// Device options
  /// Read audio waveform from this file
  wxString optionFileName = "";
  /// Read audio waveform from this device
  wxString optionDeviceName = "default";
  /// Sample rate
  long optionSampleRate = 48000;
  /// Audio device channel
  long optionChannel = 0;
  /// Audio device input buffer storage
  long optionMsLatency = 100;
  /// Pre-processor options
  /// Subsample the audio waveform after lowpass filtering
  long optionSubSample = 2;
  /// Low pass filter cutoff frequency 
  long optionInputLpFilterCutoff = 1000;
  /// High pass filter cutoff frequency 
  long optionBaseLineHpFilterCutoff = 200;
  /// Disable the high pass filter
  bool optionDisableHpFilter = false;
  /// Disable the pre-processor AGC
  bool optionDisableAgc = false;
  /// Pitch tracker options
  /// Window width for correlation 
  long optionMsWindow = 25;
  /// Maximum lag time for correlation
  long optionMsTmax = 10;
  /// Interval between pitch estimates
  long optionMsTsample = 10;
  /// Maximum threshold for the mean cumulative difference function
  double optionThreshold = 0.1;
  /// Remove DC value from correlation 
  bool optionRemoveDC = true;
  /// Nominal A4 frequency
  long optionA4Frequency = 440;
  /// Debugging options
  /// Enable driving the GUI with fake pitch estimates 
  bool optionGuiTest = false;
  /// Enable storage of debugging information
  bool optionDebug = false;
  /// GUI options
  /// Frame size in pixels
  wxSize optionFrameSize = wxSize(400,400);
} optionsPitchTuner_t;

/// \class PitchTuner
/// A class for the PitchTuner wxWidgets application
class PitchTuner : public wxApp
{
public:
  /// \brief Parse the options and create the wxWidgets GUI frame
  /// \return Success
  bool OnInit();

  /// \brief Parse the PitchTuner options.
  /// \param optionsPitchTuner PitchTuner command line options
  /// \return success
  bool ParseOptions( optionsPitchTuner_t &optionsPitchTuner );
};

DECLARE_APP(PitchTuner)
IMPLEMENT_APP(PitchTuner)

bool PitchTuner::OnInit()
{
  saSingletonInputFileSelector::Instance().
    RegisterFileType(".wav", saInputSourceOpenFileWav);

  optionsPitchTuner_t optionsPitchTuner;
  
  if ( ! ParseOptions( optionsPitchTuner ) )
    {
      return false;
    }

  PitchTunerFrame *frame =
    new PitchTunerFrame( optionsPitchTuner.optionFileName,
                         optionsPitchTuner.optionDeviceName,
                         optionsPitchTuner.optionSampleRate,
                         optionsPitchTuner.optionChannel,
                         optionsPitchTuner.optionMsLatency,
                         optionsPitchTuner.optionSubSample,
                         optionsPitchTuner.optionInputLpFilterCutoff,
                         optionsPitchTuner.optionBaseLineHpFilterCutoff,
                         optionsPitchTuner.optionDisableHpFilter,
                         optionsPitchTuner.optionDisableAgc,
                         optionsPitchTuner.optionMsWindow,
                         optionsPitchTuner.optionMsTmax,
                         optionsPitchTuner.optionMsTsample,
                         optionsPitchTuner.optionThreshold,
                         optionsPitchTuner.optionRemoveDC,
                         optionsPitchTuner.optionA4Frequency,
                         optionsPitchTuner.optionFrameSize,
                         optionsPitchTuner.optionGuiTest,
                         optionsPitchTuner.optionDebug );
  
  frame->Show(true);
  SetTopWindow(frame);
       
  return true;
}

bool PitchTuner::ParseOptions( optionsPitchTuner_t &options )
{
  static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
      { wxCMD_LINE_SWITCH, "?",  "help", "show help message",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
      { wxCMD_LINE_OPTION, NULL, "file", "input file", 
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "device", "input device", 
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "sample_rate", "sample rate", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "channel", "input device channel", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "latency_ms", "device storage latency (ms)", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "subsample", "waveform subsample ratio", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "lpcutoff", "lowpass cutoff frequency", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "hpcutoff", "highpass cutoff frequency", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_SWITCH, NULL,  "disable_hp_filter", "disable highpass filter",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_SWITCH, NULL,  "disable_agc", "disable AGC",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_OPTION, NULL, "window_ms", "correlation window width (ms)", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "lags_ms", "maximum correlation lags (ms)", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "sample_ms", "pitch estimate interval (ms)", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_OPTION, NULL, "threshold", "difference function maximum", 
        wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_SWITCH, NULL,  "removeDC", "enable DC removal",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_OPTION, NULL, "A4Frequency", "nominal A4 frequency", 
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_SWITCH, NULL,  "gui_test", "enable GUI testing",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_SWITCH, NULL, "debug", "enable debugging information",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
      { wxCMD_LINE_NONE,   NULL, NULL, NULL, 
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL }
    };

  wxCmdLineParser parser(cmdLineDesc, argc, argv);
  if (parser.Parse() > 0)
    {
      return false;
    }

  if (parser.Found("help"))
    {
      return false;
    }
  
  parser.Found("file", &(options.optionFileName));
  parser.Found("device", &(options.optionDeviceName));
  parser.Found("sample_rate", &(options.optionSampleRate));
  parser.Found("channel", &(options.optionChannel));
  parser.Found("latency_ms", &(options.optionMsLatency));
  parser.Found("lpcutoff", &(options.optionInputLpFilterCutoff));
  parser.Found("hpcutoff", &(options.optionBaseLineHpFilterCutoff));
  options.optionDisableHpFilter = parser.Found("disable_hp_filter");
  options.optionDisableAgc = parser.Found("disable_agc");
  parser.Found("window_ms", &(options.optionMsWindow));
  parser.Found("lags_ms", &(options.optionMsTmax));
  parser.Found("sample_ms", &(options.optionMsTsample));
  parser.Found("threshold", &(options.optionThreshold));
  options.optionRemoveDC = parser.Found("removeDC");
  parser.Found("A4Frequency", &(options.optionA4Frequency));
  options.optionGuiTest = parser.Found("gui_test");
  options.optionDebug = parser.Found("debug");

  if ( options.optionDebug )
    {
      std::cerr << "optionFileName "
                << options.optionFileName << std::endl;
      std::cerr << "optionDeviceName "
                << options.optionDeviceName << std::endl;
      std::cerr << "optionSampleRate "
                << options.optionSampleRate << std::endl;
      std::cerr << "optionChannel "
                << options.optionChannel << std::endl;
      std::cerr << "optionMsLatency "
                << options.optionMsLatency << std::endl;
      std::cerr << "optionSubSample "
                << options.optionSubSample << std::endl;
      std::cerr << "optionInputLpFilterCutoff "
                << options.optionInputLpFilterCutoff << std::endl;
      std::cerr << "optionBaseLineHpFilterCutoff "
                << options.optionBaseLineHpFilterCutoff << std::endl;
      std::cerr << "optionDisableHpFilter "
                << options.optionDisableHpFilter << std::endl;
      std::cerr << "optionDisableAgc "
                << options.optionDisableAgc << std::endl;
      std::cerr << "optionMsWindow "
                << options.optionMsWindow << std::endl;
      std::cerr << "optionMsTmax "
                << options.optionMsTmax << std::endl;  
      std::cerr << "optionMsTsample "
                << options.optionMsTsample << std::endl;
      std::cerr << "optionThreshold "
                << options.optionThreshold << std::endl;
      std::cerr << "optionRemoveDC "
                << options.optionRemoveDC << std::endl;
      std::cerr << "optionA4Frequency "
                << options.optionA4Frequency << std::endl;
      std::cerr << "optionGuiTest "
                << options.optionGuiTest << std::endl;
      std::cerr << "optionDebug "
                << options.optionDebug << std::endl;
      std::cerr << "optionFrameSize.GetHeight() "
                << options.optionFrameSize.GetHeight() << std::endl;
      std::cerr << "optionFrameSize.GetWidth() "
                << options.optionFrameSize.GetWidth() << std::endl;
      std::cerr << "wxScreenDC().GetPPI().GetHeight() "
                << wxScreenDC().GetPPI().GetHeight() << std::endl;
      std::cerr << "wxScreenDC().GetPPI().GetWidth() "
                << wxScreenDC().GetPPI().GetWidth() << std::endl;
    }
  
  return true;
}
