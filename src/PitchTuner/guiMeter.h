//guiMeter.h

#include <numbers>

#include "wx/wx.h"
#include <wx/dcbuffer.h>

#ifndef __GUIMETER_H__
#define __GUIMETER_H__

namespace PitchTunerGui
{
  /// \class guiMeter
  /// A class drawing the meter for the wxWidgets GUI for the pitch estimate
  class guiMeter: public wxWindow
  {
  public:
    guiMeter (wxWindow* parent, wxWindowID id, wxString name, wxString units,
              float frequency) :
      wxWindow(parent, id, wxDefaultPosition, parent->GetClientSize(),
               wxSIMPLE_BORDER),
      m_name(name),
      m_units(units),
      m_frequency(frequency),
      m_ptrMaxError(30),
      m_ptrMaxAngle(60),
      m_ptrOffScaleAngle(70),
      m_guiDialBitmap(0),
      m_pitchError(0.0),
      m_pitch(0.0),
      m_voiced(false),
      m_ptrOriginX(0),
      m_ptrOriginY(0),
      m_dialRadius(0),
      m_dotLeftX(0),
      m_dotRightX(0),
      m_dotY(0),
      m_dotRadius(0),
      m_w(0),
      m_h(0)
    {
      // For wxAutoBufferedPaintDC()
      SetBackgroundStyle(wxBG_STYLE_PAINT);
    }

    ~guiMeter() throw()
    {
      delete m_guiDialBitmap;
    }
    
    void SetPitchError(const float& pitchError)
    {
      m_pitchError = pitchError;
    }
    
    void SetPitch(const float& pitch, const bool& voiced)
    {
      m_pitch = pitch; m_voiced = voiced;
    }
    
    void SetFrequency(const float& frequency)
    {
      m_frequency = frequency;
    }
    
    void SetUnits(const wxString& units)
    {
      m_units = units;
    }
    
    void SetName(const wxString& name)
    {
      m_name = name;
    }
    
    void RePaint()
    {
      Refresh(); Update();
    }
    
    
  private:
    // Disallow copy constructor of guiMeter
    guiMeter(const guiMeter&);
    // Disallow assignment to guiMeter
    guiMeter& operator=(const guiMeter&);

    void OnPaint(wxPaintEvent& event);
    void DrawDial(wxMemoryDC &guiDialDC);
    void DrawUpdate(wxAutoBufferedPaintDC &dc);

    wxString m_name;
    wxString m_units;
    float m_frequency;
    int m_ptrMaxError;
    int m_ptrMaxAngle;
    int m_ptrOffScaleAngle;
    wxBitmap* m_guiDialBitmap;
    float m_pitchError;
    float m_pitch;
    bool m_voiced;
    int m_ptrOriginX;
    int m_ptrOriginY;
    int m_dialRadius;
    wxCoord m_dotLeftX;
    wxCoord m_dotRightX;
    wxCoord m_dotY;
    wxCoord m_dotRadius;
    int m_w;
    int m_h;
    wxFont fontUnits;
    wxFont fontTicks;
    wxFont fontName;
    wxFont fontFrequency;
    wxFont fontPitch;

    const double pi = std::numbers::pi_v<double>;

    DECLARE_CLASS( guiMeter )
    DECLARE_EVENT_TABLE()
  };

}
#endif

