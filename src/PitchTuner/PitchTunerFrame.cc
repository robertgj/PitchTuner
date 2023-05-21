/// \file PitchTunerFrame.cc
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
#include "PitchTunerFrame.h"
#include "guiOptionsDialog.h"
#include "guiDeviceInfo.h"
#include "guiMeter.h"

// The original icon is at https://www.flaticon.com/free-icon/tuning-fork_1005015
#include "PitchTuner.xpm"

// Use "#" or the hash to represent the sharp. 
// In fact sharp is \u266F flat is \u266D
const wxChar* 
PitchTunerFrame::m_noteCharStrings[PitchTunerFrame::m_numNotes] = 
  {
    wxT("A5"), wxT("G5\u266F"), wxT("G5"), wxT("F5\u266F"), wxT("F5"), 
    wxT("E5"), wxT("D5\u266F"), wxT("D5"), wxT("C5\u266F"), wxT("C5"), 
    wxT("B4"), wxT("A4\u266F"), wxT("A4"), wxT("G4\u266F"), wxT("G4"), 
    wxT("F4\u266F"), wxT("F4"), wxT("E4"), wxT("D4\u266F"), wxT("D4"), 
    wxT("C4\u266F"), wxT("C4"), wxT("B3"), wxT("A3\u266F"), wxT("A3")
  };
const int PitchTunerFrame::m_noteSemitonesFromA4[PitchTunerFrame::m_numNotes] = 
  {
    12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,  
    0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12 
  };
const float PitchTunerFrame::m_semitoneRatio = powf(2.0f, 1.0f/12.0f);
const float PitchTunerFrame::m_1200onLog2 = 1200.0f/logf(2.0f);

enum { ID_Pitch = wxID_HIGHEST+1,
       ID_EOS,
       ID_ThreadError,
       ID_GuiTest,
       ID_DeviceOptions,
       ID_About,
       ID_Quit};

DECLARE_EVENT_TYPE(wxEVT_PITCH_EVENT, -1)
DEFINE_EVENT_TYPE(wxEVT_PITCH_EVENT)

DECLARE_EVENT_TYPE(wxEVT_EOS_EVENT, -1)
DEFINE_EVENT_TYPE(wxEVT_EOS_EVENT)

DECLARE_EVENT_TYPE(wxEVT_THREAD_EVENT, -1)
DEFINE_EVENT_TYPE(wxEVT_THREAD_EVENT)

BEGIN_EVENT_TABLE(PitchTunerFrame, wxFrame)
EVT_COMMAND(ID_Pitch, wxEVT_PITCH_EVENT, PitchTunerFrame::OnPitch)
EVT_COMMAND(ID_EOS, wxEVT_EOS_EVENT, PitchTunerFrame::OnEndOfStream)
EVT_COMMAND(ID_ThreadError, wxEVT_THREAD_EVENT, PitchTunerFrame::OnThreadError)
EVT_TIMER(ID_GuiTest, PitchTunerFrame::OnGuiTest)
EVT_MENU(ID_DeviceOptions, PitchTunerFrame::OnDeviceOptions)
EVT_MENU(ID_About, PitchTunerFrame::OnAbout)
EVT_MENU(ID_Quit, PitchTunerFrame::OnQuit)
EVT_CLOSE(PitchTunerFrame::OnClose)
END_EVENT_TABLE()

PitchTunerFrame::PitchTunerFrame( wxString fileName,
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
                                  bool debug ) 
  : wxFrame((wxFrame *)NULL, wxID_ANY, wxT("PitchTuner"),
            wxDefaultPosition,
            frameSize,
            wxCLIP_CHILDREN|wxRESIZE_BORDER|
            wxSYSTEM_MENU|wxCAPTION|wxCLOSE_BOX),
  m_fileName(fileName),
  m_deviceName(deviceName),
  m_sampleRate(sampleRate),
  m_deviceLatencyMs(msLatency),
  m_channel(channel),
  m_subSample(subSample),
  m_inputLpFilterCutoff(inputLpFilterCutoff),
  m_baseLineHpFilterCutoff(baseLineHpFilterCutoff),
  m_disableHpFilter(disableHpFilter),
  m_disableAgc(disableAgc),
  m_msWindow(msWindow),
  m_msTmax(msTmax),
  m_msTsample(msTsample),
  m_threshold(threshold),
  m_removeDC(removeDC),
  m_A4Frequency(A4Frequency),
  m_guiTest(guiTest),
  m_debug( debug ),
  m_deviceFound( false ),
  m_A4FrequencyLower(420),
  m_A4FrequencyUpper(460),
  m_frequency(0.0),
  m_units(wxString(wxT("cents"))),
  m_noteNames(m_numNotes, m_noteCharStrings),
  m_noteNameIndex(m_A4NoteNameIndex),
  m_deviceInfo(),
  m_deviceInfoIndex(0),
  m_deviceSampleRateIndex(0),
  m_pitchSample(0.0),
  m_voiced(false),
  m_meter(0),
  m_runThread(false),
  m_guiMutex(),
  m_guiEvent(wxEVT_PITCH_EVENT, ID_Pitch),
  m_threadEvent(wxEVT_THREAD_EVENT, ID_ThreadError),
  m_eosEvent(wxEVT_EOS_EVENT, ID_EOS),
  m_EndOfStream( false ),
  m_guiTestTimerIntervalMs(2000),
  m_guiTestTimer()
{
  // Define an event used to trigger Gui updates
  m_guiEvent.SetEventObject(this);
  m_threadEvent.SetEventObject(this);
  m_eosEvent.SetEventObject(this);

  // Set note ratios
  for (int noteIndex=0; noteIndex<m_numNotes; noteIndex++)
    {
      m_noteRatioFromA4[noteIndex] = 
        powf(m_semitoneRatio, (float)m_noteSemitonesFromA4[noteIndex]);
    }
  m_frequency = m_noteRatioFromA4[m_noteNameIndex]*(float)m_A4Frequency;

  // Create gui elements
  CreateGui();

  if(m_guiTest)
    {
      m_guiTestTimer.SetOwner(GetEventHandler(), ID_GuiTest);
      m_guiTestTimer.Start( m_guiTestTimerIntervalMs, wxTIMER_CONTINUOUS );
    }
  
  // Find available audio devices
  m_deviceFound = FindAudioDevices();
  if ( m_deviceFound )
    {
      // Start the pitch sampling thread
      RunThread();
    }
  else
    {
      wxMessageBox("", wxT("No audio device!"), wxICON_EXCLAMATION|wxOK);
    }
}

void PitchTunerFrame::HandleError(const wxString errStr)
{
  wxMessageBox(errStr, wxT("Error!"), wxICON_EXCLAMATION|wxOK);
  Close();
}

bool PitchTunerFrame::FindAudioDevices()
{
  // Query devices
  SimpleAudio::saDeviceInfo saDeviceInfo;
  saDeviceInfo.queryInputDevices();
  if (saDeviceInfo.Empty())
    {
      // Can't call Destroy() in constructor!
      return false;
    }

  // Number of devices?
  SimpleAudio::saDeviceInfoDeviceNameList saDeviceNameList;
  saDeviceInfo.getDeviceNames(saDeviceNameList);  
  std::size_t numDevices = saDeviceNameList.size();
  m_deviceInfo.Alloc(numDevices);

  // Copy devices
  for (std::size_t deviceNum=0; deviceNum<numDevices; deviceNum++)
    {
      SimpleAudio::saDeviceInfoSampleRateList saSampleRateList;
      saDeviceInfo.getDeviceSampleRateList(saDeviceNameList[deviceNum], 
                                           saSampleRateList);
      m_deviceInfo.AddDevice(saDeviceNameList[deviceNum], saSampleRateList);
    }

  return true;
}

void PitchTunerFrame::CreateGui()
{  
  // set the frame icon
  SetIcon(wxICON(PitchTuner));
  
  // Create a menu bar
  wxMenuBar *menuBar = new wxMenuBar;
  wxMenu *optionsMenu = new wxMenu;
  optionsMenu->Append( ID_DeviceOptions, wxT("&Device Options") );
  optionsMenu->Append( ID_About, wxT("&About") );
  optionsMenu->Append( ID_Quit, wxT("&Quit") );
  menuBar->Append( optionsMenu, wxT("&Options") );
  SetMenuBar( menuBar );

  // Initialise m_meter AFTER creating menu bar so client area is correct
  try
    {
      m_meter = new guiMeter
        (this, wxID_ANY, m_noteNames[m_noteNameIndex], m_units, m_frequency);
    }
  catch (std::bad_alloc& excpt)
    {
      HandleError(wxT("Memory allocation failed initialising guiMeter()"));
    }
}

void
PitchTunerFrame::UpdateGui()
{
  // Calculate and set the current pitch error
  {
    // Lock data shared with the pitch tracker thread
    wxMutexLocker lock(m_guiMutex);

    // Set the note name and frequency displayed on the meter
    if (m_voiced)
      {
        // Estimate the closest semitone in the equal-tempered scale
        int index_offset =
          (int)round(m_1200onLog2*logf(m_pitchSample/m_A4Frequency)/100.0);
        m_noteNameIndex = m_A4NoteNameIndex - index_offset;

        if (m_noteNameIndex < 0)
          {
            m_noteNameIndex=0;
          }
        if (m_noteNameIndex >= m_numNotes)
          {
            m_noteNameIndex=m_numNotes-1;
          }
        m_frequency = m_noteRatioFromA4[m_noteNameIndex]*(float)m_A4Frequency;
        m_meter->SetFrequency(m_frequency);
        m_meter->SetName(m_noteNames[m_noteNameIndex]);
      }

    // Pitch error in cents (100ths of a semitone)
    m_meter->SetPitch(m_pitchSample, m_voiced);
    float pitchError = m_1200onLog2*logf(m_pitchSample/m_frequency);
    m_meter->SetPitchError(pitchError);
  }

  // Paint the meter
  m_meter->RePaint();
}

void PitchTunerFrame::OnDeviceOptions(wxCommandEvent& WXUNUSED(event))
{
  // Stop the pitch sampling thread
  StopThread();

  // Create dialog
  guiOptionsDialog optionsDialog(this,
                                 wxString(wxT("Options")), 
                                 m_deviceInfo,
                                 m_deviceInfoIndex,
                                 m_deviceSampleRateIndex,
                                 m_A4FrequencyLower,
                                 m_A4FrequencyUpper,
                                 m_A4Frequency);

  // Show dialog
  if ( optionsDialog.ShowModal() == wxID_OK )
    {
      // Set device
      m_deviceInfoIndex = optionsDialog.GetDeviceNameIndex();
      m_deviceSampleRateIndex = optionsDialog.GetDeviceSampleRateIndex();
      
      // Set A4 frequency on meter
      m_A4Frequency = optionsDialog.GetA4Frequency();
    } 
 
  // Start the pitch sampling thread
  RunThread();
}

void PitchTunerFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(wxT("PitchTuner is based on: \"YIN, A fundamental frequency\n"
                   "estimator for speech and music\", A. de Cheveigne and\n"
                   "H. Kawahara, Journal of the Acoustical Society of America,\n"
                   "Vol. 11, No. 4, April 2002, pp. 1917-1930. \n"
                   "\n"
                   "The GUI uses wxWidgets: https://www.wxwidgets.org\n"
                   "\n"
                   "The original icon is at:\n"
                   "https://www.flaticon.com/free-icon/tuning-fork_1005015\n"
                   "\n"
                   "Copyright (C) 2023 Robert G. Jenssen\n"
                   "\n"
                   "Permission is hereby granted, free of charge, to any \n"
                   "person obtaining a copy of this software and associated\n"
                   "documentation files (the “Software”), to deal in the\n"
                   "Software without restriction, including without\n"
                   "limitation the rights to use, copy, modify, merge,\n"
                   "publish, distribute, sublicense, and/or sell copies of\n"
                   "the Software, and to permit persons to whom the\n"
                   "Software is furnished to do so, subject to the\n"
                   "following conditions:\n"
                   "\n"
                   "The above copyright notice and this permission notice\n"
                   "shall be included in all copies or substantial portions\n"
                   "of the Software.\n"
                   "\n"
                   "THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT \n"
                   "WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, \n"
                   "INCLUDING BUT NOT LIMITED TO THE WARRANTIES\n"
                   "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR\n"
                   "PURPOSE AND NONINFRINGEMENT. IN NO EVENT \n"
                   "SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE \n"
                   "LIABLE FOR ANY CLAIM, DAMAGES OR OTHER \n"
                   "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, \n"
                   "TORT OR OTHERWISE, ARISING FROM, OUT OF OR \n"
                   "IN CONNECTION WITH THE SOFTWARE OR THE USE \n"
                   "OR OTHER DEALINGS IN THE SOFTWARE."
                   ),
               wxT("About PitchTuner"),
               wxOK|wxICON_INFORMATION, this);
}

void PitchTunerFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close();
}

void PitchTunerFrame::OnEndOfStream(wxCommandEvent& WXUNUSED(event))
{
  Close();
}

void PitchTunerFrame::OnThreadError(wxCommandEvent& WXUNUSED(event))
{
  HandleError(m_threadErrStr);
}

void PitchTunerFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
  StopThread();
  Destroy();

  // Needed to send a GUI event?
  ::wxWakeUpIdle();
}

void PitchTunerFrame::OnPitch(wxCommandEvent& WXUNUSED(event))
{
  UpdateGui();

  if ( m_debug && m_voiced )
    {
      std::cerr << m_pitchSample << std::endl;
    }
}

void PitchTunerFrame::OnGuiTest(wxTimerEvent& WXUNUSED(event))
{
  typedef struct _TestSamples_t
  { const float pitch; bool voiced; const char *comment;} TestSamples_t;
  static const TestSamples_t pitch_samples[] = {
    { 261.79, true, "estimate of timidity C4" }, 
    { 293.27, true, "estimate of timidity D4" },
    { 327.95, true, "estimate of timidity E4" },
    { 349.38, true, "estimate of timidity F4" },
    { 392.18, true, "estimate of timidity G4" },
    { 439.97, true, "estimate of timidity A4" },
    { 494.69, true, "estimate of timidity B4" },
    { 523.83, true, "estimate of timidity C5" },
    { 257.88, true, "C4 25 cents low" },
    { 265.44, true, "C4 25 cents high" },
    { 433.69, true, "A4 25 cents low" },
    { 446.40, true, "A4 25 cents high" },
    {      0, false, "no pitch" },
  };
  static unsigned pitch_index = 0;

  {
    wxMutexLocker lock(m_guiMutex);
    m_voiced = pitch_samples[pitch_index].voiced;
    m_pitchSample = pitch_samples[pitch_index].pitch;
  }
  
  UpdateGui();

  if (m_debug)
    {
      std::cerr << wxString::Format(wxT("OnGuiTest: m_pitchSample = %6.2f"
                                        " m_frequency = %6.2f"
                                        " m_noteNames[] = %s : %s\n"),
                                    m_pitchSample,
                                    m_frequency,
                                    m_noteNames[m_noteNameIndex],
                                    pitch_samples[pitch_index].comment);
    }
  
  pitch_index += 1;
  if (pitch_index >= sizeof(pitch_samples)/sizeof(TestSamples_t))
    {
      pitch_index = 0;
    }
}

void PitchTunerFrame::RunThread()
{
  if (m_guiTest) 
    {
      return;
    }

  // Start the thread
  if (wxThreadHelper::CreateThread() != wxTHREAD_NO_ERROR)
    {
      HandleError(wxString("failed to create thread"));
    }

  m_runThread = true;
  wxThreadHelper::GetThread()->SetPriority(WXTHREAD_MIN_PRIORITY);
  if (wxThreadHelper::GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
      HandleError(wxString("failed to run thread"));
    }

  if (m_debug)
    {
      std::cerr << wxString::Format(wxT("Running thread\n"));
    }
}

void PitchTunerFrame::StopThread()
{
  if (m_guiTest) 
    {
      return;
    }

  // Stop the thread
  if ( wxThreadHelper::GetThread()->IsRunning() )
    {
      m_runThread = false;
      // Race possible here?
      wxThreadHelper::GetThread()->Wait();
    }

  if (m_debug)
    {
      std::cerr << wxString::Format(wxT("Thread stopped\n"));
    }
}

wxThread::ExitCode PitchTunerFrame::Entry()
{
  // Prevent exceptions being passed to wxWidgets
  try
    {
      // Open the audio input stream
      wxString sourceName;
      if (m_fileName.Length())
        {
          sourceName = m_fileName;
        }
      else if (m_deviceName.Length())
        {
          sourceName = m_deviceName;
        }
      else
        {
          sourceName = m_deviceInfo.GetDeviceName(m_deviceInfoIndex);
        }

      // Open stream
      std::string name(sourceName.c_str());
      std::size_t sampleRate = m_deviceInfo.GetDeviceSampleRate
        (m_deviceInfoIndex, m_deviceSampleRateIndex);
      
      saInputStream* inputStreamPtr;
      if ( m_fileName.IsEmpty() )
        {
          inputStreamPtr = SimpleAudio::saInputStreamOpenDevice
            (name, sampleRate, m_deviceLatencyMs);
        }
      else
        {
          inputStreamPtr = SimpleAudio::saInputStreamOpenFile(name);
        }

      std::unique_ptr<SimpleAudio::saInputStream> inputStream(inputStreamPtr);

      if ( m_debug )
        {
          std::cerr << wxString::Format(wxT("Opening %s, sample rate %ld\n"), 
                                        sourceName.c_str(), sampleRate);
        }

      sampleRate = inputStream->GetFramesPerSecond();

      // Create a pitch tracker for the input stream

      // Create PreProcessor
      std::unique_ptr<PreProcessor> 
        pre( PreProcessorCreate( inputStream.get(), m_channel, 
                                 m_msWindow, m_msTmax, m_msTsample, m_subSample,
                                 m_inputLpFilterCutoff, m_baseLineHpFilterCutoff,
                                 m_disableHpFilter, m_disableAgc, m_debug ) );

      // Create PitchTracker
      std::unique_ptr<PitchTracker> 
        tracker( PitchTrackerCreate
                 ( pre->SubSampleRate(), pre->WindowLength(), pre->MaxLags(),
                   m_threshold, m_removeDC, m_debug ) );

      // Read from the pitch tracker
      while ( m_runThread )
        {
          // Sanity check
          if ( inputStream->EndOfSource() )
            {
              if ( m_fileName.IsEmpty() )
                {
                  throw std::runtime_error("Unexpected end-of-stream!");
                }
              else
                {
                  m_EndOfStream = true;
                  GetEventHandler()->AddPendingEvent( m_eosEvent );
                  break;
                }
            }

          // Get pitch sample
          float sample = tracker->EstimatePitch( pre->Read() );

          // Set pitch sample
          {
            //Locking scope
            wxMutexLocker lock(m_guiMutex);
            m_pitchSample = sample;
            m_voiced = tracker->Voiced();
          }

          // Send pitch event to Gui thread. 
          // Could I pass the pitch in the event?
          GetEventHandler()->AddPendingEvent(m_guiEvent);
        }
      // pre, tracker and inputStream are destroyed here
    }
  catch(std::exception& excpt)
    {
      m_threadErrStr = wxString(excpt.what(), *wxConvCurrent);
      GetEventHandler()->AddPendingEvent(m_threadEvent);
    }

  // Yield() to try and avoid race condition in between main 
  // thread reset of m_runThread then Wait()
  wxThreadHelper::GetThread()->Yield();

  //Done
  return (wxThread::ExitCode)0;
}

