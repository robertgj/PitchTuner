// guiOptionsDialog.cc

#include <wx/wx.h>

#include "guiOptionsDialog.h"
#include "guiDeviceInfo.h"

using namespace PitchTunerGui;
  
// guiOptionsDialog type definition for wxWidgets
IMPLEMENT_CLASS( guiOptionsDialog, wxDialog )

guiOptionsDialog::guiOptionsDialog(wxFrame* parent, 
                                   const wxString& caption,
                                   const guiDeviceInfo& deviceInfo, 
                                   const int deviceNameIndex, 
                                   const int deviceSampleRateIndex,
                                   const int A4FrequencyLower, 
                                   const int A4FrequencyUpper, 
                                   const int A4Frequency) :
wxDialog(parent, wxID_ANY, caption, 
         parent->GetPosition(),
         parent->GetClientSize(),
         wxSTAY_ON_TOP|wxCAPTION),
  m_deviceNameCtrl(0), 
  m_sampleRateCtrl(0), 
  m_A4FrequencyCtrl(0),
  m_deviceNameIndex(deviceNameIndex), 
  m_sampleRateIndex(deviceSampleRateIndex),
  m_A4Frequency(A4Frequency)
{
  // A sizer for device name selection 
  wxBoxSizer* deviceNameSizer = new wxBoxSizer(wxHORIZONTAL);

  // Label for the device name choice control
  wxStaticText* deviceNameLabel = 
    new wxStaticText(this, wxID_STATIC, wxT(" Device name:"));
  deviceNameSizer->Add(deviceNameLabel);
  deviceNameSizer->AddStretchSpacer();

  // Get the array of device name strings
  wxArrayString deviceNameStrings;
  deviceInfo.GetDeviceNameList(deviceNameStrings);

  // Create a choice control for the device name
  m_deviceNameCtrl = new wxChoice
    (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, deviceNameStrings); 
  m_deviceNameCtrl->SetStringSelection(deviceNameStrings[deviceNameIndex]);
  deviceNameSizer->Add(m_deviceNameCtrl);

  // A sizer for device sample rate selection 
  wxBoxSizer* sampleRateSizer = new wxBoxSizer(wxHORIZONTAL);

  // Label for the device sample rate choice control
  wxStaticText* sampleRateLabel = 
    new wxStaticText(this, wxID_STATIC, wxT(" Sample Rate:"));
  sampleRateSizer->Add(sampleRateLabel);
  sampleRateSizer->AddStretchSpacer();

  // Create the sample rate choice control

  // Create an array of sample rate strings
  std::vector<std::size_t> sampleRateList;
  deviceInfo.GetDeviceSampleRateList(deviceNameStrings[m_deviceNameIndex], 
                                     sampleRateList);
  wxArrayString sampleRateStrings;
  std::vector<std::size_t>::iterator rate;
  for (rate = sampleRateList.begin(); rate != sampleRateList.end(); rate++)
    {
      sampleRateStrings.Add(wxString::Format(wxT("%lu"), *rate));
    }

  // Create the  control
  m_sampleRateCtrl = new wxChoice
    (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, sampleRateStrings); 
  m_sampleRateCtrl->SetStringSelection(sampleRateStrings[m_sampleRateIndex]);
  sampleRateSizer->Add(m_sampleRateCtrl);

  // A sizer for the A4 frequency control
  wxBoxSizer* A4Sizer = new wxBoxSizer(wxHORIZONTAL);

  // Label for the A4 frequency choice control
  wxStaticText* A4FrequencyLabel = 
    new wxStaticText(this, wxID_STATIC, wxT(" A4 Frequency:"));
  A4Sizer->Add(A4FrequencyLabel);
  A4Sizer->AddStretchSpacer();

  // Create the A4 frequency choice control
  m_A4FrequencyCtrl = new wxSpinCtrl
    (this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize,
     wxSP_VERTICAL|wxSP_ARROW_KEYS,
     A4FrequencyLower, A4FrequencyUpper, m_A4Frequency);
  A4Sizer->Add(m_A4FrequencyCtrl);

  // Add OK and Cancel buttons
  wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  wxButton* cancelButton = new wxButton(this, wxID_CANCEL);
  cancelButton->Connect(wxID_CANCEL, 
                        wxEVT_COMMAND_BUTTON_CLICKED,
                        wxCommandEventHandler(guiOptionsDialog::OnCancel),
                        NULL, this);
  buttonSizer->Add(cancelButton); 
  buttonSizer->AddStretchSpacer();
  wxButton* okButton = new wxButton(this, wxID_OK);
  okButton->Connect(wxID_OK, 
                    wxEVT_COMMAND_BUTTON_CLICKED,
                    wxCommandEventHandler(guiOptionsDialog::OnOk), 
                    NULL, this);
  buttonSizer->AddStretchSpacer();
  buttonSizer->Add(okButton);

  // A sizer for the panel
  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->AddSpacer(2);
  topSizer->Add(deviceNameSizer, wxSizerFlags(0).Expand());
  topSizer->AddSpacer(2);
  topSizer->Add(sampleRateSizer, wxSizerFlags(0).Expand());
  topSizer->AddSpacer(2);
  topSizer->Add(A4Sizer, wxSizerFlags(0).Expand());
  topSizer->AddSpacer(4);
  topSizer->Add(buttonSizer, wxSizerFlags(0).Expand());
  topSizer->AddSpacer(2);

  // Fit the dialog to the minimum size dictated by the sizers
  SetSizer(topSizer);
  topSizer->SetSizeHints(this);
}

guiOptionsDialog::~guiOptionsDialog()
{
}

void 
guiOptionsDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
  m_deviceNameIndex = m_deviceNameCtrl->GetSelection();
  m_sampleRateIndex = m_sampleRateCtrl->GetSelection();
  m_A4Frequency = m_A4FrequencyCtrl->GetValue();
  EndModal(wxID_OK);
}

void 
guiOptionsDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}
