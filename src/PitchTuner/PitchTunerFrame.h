/// \file PitchTunerFrame.h
/// \brief wxWidgets GUI for the PitchTracker pitch estimator

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
#include "guiOptionsDialog.h"
#include "guiDeviceInfo.h"
#include "guiMeter.h"

using namespace PitchTunerGui;

/// \class PitchTunerFrame
/// A class for the PitchTuner wxWidgets GUI
class PitchTunerFrame : public wxFrame, public wxThreadHelper
{  
public:
  /// \brief Constructor
  /// \param fileName Read audio waveform from this file
  /// \param deviceName Read audio waveform from this device
  /// \param sampleRate Sample rate
  /// \param channel Audio input device channel
  /// \param msLatency Audio device input buffer storage
  /// \param subSample Subsample the audio waveform after lowpass filtering
  /// \param inputLpFilterCutoff Low pass filter cutoff frequency 
  /// \param baseLineHpFilterCutoff High pass filter cutoff frequency 
  /// \param disableHpFilter Disable the high pass filter
  /// \param disableAgc Disable the pre-processor AGC
  /// \param msWindow Window width for correlation 
  /// \param msTmax Maximum lag time for correlation
  /// \param msTsample Interval between pitch estimates
  /// \param threshold Maximum threshold for the difference function
  /// \param removeDC Remove DC value from correlation 
  /// \param A4Frequency Nominal A4 frequency 
  /// \param frameSize Frame size in pixels
  /// \param guiTest Enable driving the GUI with fake pitch estimates 
  /// \param debug Enable storage of debugging information
  PitchTunerFrame( wxString fileName,
                   wxString deviceName,
                   float sampleRate,
                   std::size_t channel,
                   std::size_t msLatency,
                   std::size_t subSample,
                   float inputLpFilterCutoff,
                   float baseLineHpFilterCutoff,
                   bool disableHpFilter,
                   bool disableAgc,
                   std::size_t msWindow,
                   std::size_t msTmax,
                   std::size_t msTsample,
                   float threshold,
                   bool removeDC, 
                   float A4Frequency,
                   wxSize frameSize,
                   bool guiTest,
                   bool debug );
  
  ~PitchTunerFrame() throw() { delete m_meter; }

private:
  /// Read audio waveform from this file
  wxString m_fileName;

  /// Read audio waveform from this device
  wxString m_deviceName;
  
  /// Audio input device sample rate
  float m_sampleRate;

  /// Audio device input buffer storage in ms
  std::size_t m_deviceLatencyMs;

  /// Audio input device channel
  std::size_t m_channel;

  /// Subsample the audio waveform after lowpass filtering
  std::size_t m_subSample;

  /// Low pass filter cutoff frequency 
  float m_inputLpFilterCutoff;

  /// High pass filter cutoff frequency 
  float m_baseLineHpFilterCutoff;

  /// Disable the high pass filter
  bool m_disableHpFilter;

  /// Disable the pre-processor AGC
  bool m_disableAgc;

  /// Window width for correlation 
  std::size_t m_msWindow;

  /// Maximum lag time for correlation
  std::size_t m_msTmax;

  /// Interval between pitch estimates
  std::size_t m_msTsample;

  /// Maximum threshold for the difference function
  float m_threshold;

  /// Remove DC value from correlation 
  bool m_removeDC;
  
  /// Nominal A4 frequency
  int m_A4Frequency;

  /// Enable GUI debugging with a fake pitch estimate
  bool m_guiTest;

  /// Store debugging information
  bool m_debug;

  /// Device found 
  bool m_deviceFound;

  /// Meter lower valid A4 frequency
  int m_A4FrequencyLower;

  /// Meter upper valid A4 frequency
  int m_A4FrequencyUpper;

  /// Note frequency
  float m_frequency;

  /// Meter units
  wxString m_units;

  /// List of note names
  wxArrayString m_noteNames;
  int m_noteNameIndex;

  /// Timer to update meter
  wxTimer m_timer;

  /// Device info lists
  guiDeviceInfo m_deviceInfo;

  /// Index of current device 
  int m_deviceInfoIndex;

  /// Index of sample rate for current device 
  int m_deviceSampleRateIndex;

  /// Pitch sample
  float m_pitchSample;
  
  /// Voiced?
  bool m_voiced;

  /// Meter
  guiMeter* m_meter;

  /// Thread running flag
  bool m_runThread;

  /// Mutex on pitch
  wxMutex m_guiMutex;

  /// Event on pitch
  wxCommandEvent m_guiEvent;

  /// Event on thread error
  wxCommandEvent m_threadEvent;

  /// Event on end-of-stream
  wxCommandEvent m_eosEvent;

  /// Usually end-of-file for an input stream from a file
  bool m_EndOfStream;

  // Options

  /// Run the GUI test timer and show artificial pitch estimates
  bool GuiTest;

  /// Timer interval for the GUI test
  std::size_t m_guiTestTimerIntervalMs;

  /// wxWidgets timer object
  wxTimer m_guiTestTimer;

  /// Thread error message
  wxString m_threadErrStr;

  /// Pitch estimation thread entry function called by wxThreadHelper
  void* Entry();
    
  /// Number of notes
  static const int m_numNotes = 25;

  /// List of names of notes
  static const wxChar* m_noteCharStrings[m_numNotes];

  /// List of semitones from A4 for each note
  static const int m_noteSemitonesFromA4[m_numNotes];

  /// List of frequency ratio of note compared to A4
  float m_noteRatioFromA4[m_numNotes];
  
  /// Index of A4
  static const int m_A4NoteNameIndex = 12;

  /// Frequency ratio of a semitone in 12-TET
  static const float m_semitoneRatio;

  /// Frequency ratio of a cent
  static const float m_1200onLog2;
  
  /// Disallow assignment to PitchTunerFrame
  PitchTunerFrame& operator=( const PitchTunerFrame& );
  
  /// Disallow copy constructor of PitchTunerFrame
  PitchTunerFrame( const PitchTunerFrame& );

  /// Device s menu item selected. Show the "Device Options" dialog box
  void OnDeviceOptions(wxCommandEvent&);
  
  /// About menu item selected. Show the "about" message box
  void OnAbout(wxCommandEvent&);

  /// Quit menu item selected
  void OnQuit(wxCommandEvent&);

  /// A new pitch estimate is available. Update the GUI
  void OnPitch(wxCommandEvent&);

  /// End of stream event found on file or device
  void OnEndOfStream(wxCommandEvent&);

  /// Error in the pitch estimation thread
  void OnThreadError(wxCommandEvent&);

  /// Close button on frame selected
  void OnClose(wxCloseEvent&);

  /// The GUI test waits for a timer event before updating the GUI with a
  /// fake pitch estimate
  void OnGuiTest(wxTimerEvent&);

  /// Create the wxWidgets GUI elements: meter graphic, option  menu and dialog
  void CreateGui();
  
  /// Draw the wxWidgets GUI
  void UpdateGui();

  /// Interrogate the sound system and create a list of audio devices
  bool FindAudioDevices();

  /// Run the pitch estimation algorithm in a separate thread
  void RunThread();

  /// Stop the pitch estimation thread
  void StopThread();

  /// Show an error message string on a message box
  void HandleError(const wxString);

  DECLARE_EVENT_TABLE()
};
