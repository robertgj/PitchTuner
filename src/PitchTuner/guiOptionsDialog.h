/// \file guiOptionsDialog.h
/// \brief PitchTuner GUI options

#include <vector>

#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>

#include "guiDeviceInfo.h"

#ifndef __GUIOPTIONSDIALOG_H__
#define __GUIOPTIONSDIALOG_H__

namespace PitchTunerGui
{
  /// \class guiOptionsDialog
  /// \brief A class for the wxWidgets GUI options dialog
  class guiOptionsDialog : public wxDialog
  {
  public:
    /// \brief Constructor
    /// \param parent parent frame
    /// \param caption caption
    /// \param deviceInfo list of devices
    /// \param deviceNameIndex initial device name index
    /// \param deviceSampleRateIndex initial device sample rate index
    /// \param A4FrequencyLower lower A4 frequency
    /// \param A4FrequencyUpper upper A4 frequency
    /// \param A4Frequency initial A4 frequency 
    guiOptionsDialog(wxFrame* parent, 
                     const wxString& caption,
                     const guiDeviceInfo& deviceInfo, 
                     const int deviceNameIndex, 
                     const int deviceSampleRateIndex,
                     const int A4FrequencyLower, 
                     const int A4FrequencyUpper, 
                     const int A4Frequency);

    ~guiOptionsDialog();

    /// \brief Get selected device name index
    /// \return device name index
    int GetDeviceNameIndex() { return m_deviceNameIndex; }

    /// \brief Get selected device sample rate index
    /// \return device sample rate index
    int GetDeviceSampleRateIndex() { return m_sampleRateIndex; }

    /// \brief Get selected A4 frequency index
    /// \return A4 frequency index
    int GetA4Frequency() { return m_A4Frequency; }

  private:
    
    /// Choose device name
    wxChoice* m_deviceNameCtrl;

    /// Choose sample rate
    wxChoice* m_sampleRateCtrl;

    /// Choose A4 frequency
    wxSpinCtrl* m_A4FrequencyCtrl;

    // Data members initialised before and after dialog display
    int m_deviceNameIndex;
    int m_sampleRateIndex;
    int m_A4Frequency;

    void OnOk(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);

    // Disallow copy constructor of guiOptionsDialog
    guiOptionsDialog(const guiOptionsDialog&);

    // Disallow assignment to guiOptionsDialog
    guiOptionsDialog& operator=(const guiOptionsDialog&);

    DECLARE_CLASS( guiOptionsDialog )
  };

}

#endif

