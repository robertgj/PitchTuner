// guiMeter.cc

#include <stdexcept>
#include <numbers>
#include <math.h>

#include <wx/dcbuffer.h>

#include "guiMeter.h"

using namespace PitchTunerGui;

IMPLEMENT_CLASS( guiMeter, wxWindow )

BEGIN_EVENT_TABLE( guiMeter, wxWindow )
EVT_SIZE( guiMeter::OnSize )
EVT_PAINT( guiMeter::OnPaint )
END_EVENT_TABLE()

void guiMeter::RePaint()
{
  Refresh();
  Update();
}
    
void guiMeter::OnSize(wxSizeEvent &WXUNUSED(event))
{
  RePaint();
}

void guiMeter::OnPaint( wxPaintEvent &WXUNUSED(event) )
{

  // Draw the fixed dial elements
  if ( (m_w != GetParent()->GetClientSize().GetWidth()) ||
       (m_h != GetParent()->GetClientSize().GetWidth()) )
    {
      if (m_guiDialBitmap)
        {
          delete m_guiDialBitmap;
        }

      InitGuiDial();

      m_guiDialBitmap = new wxBitmap(m_w, m_h);

      wxMemoryDC guiDialDC;
      guiDialDC.SelectObject(*m_guiDialBitmap);
      DrawGuiDial(guiDialDC);
    }
  
  // Draw the updated dial elements
  wxAutoBufferedPaintDC guiMeterDC( this );
  guiMeterDC.DrawBitmap(*m_guiDialBitmap, 0, 0);
  DrawGuiUpdate(guiMeterDC);
}

void guiMeter::InitGuiDial()
{
  m_w = GetParent()->GetClientSize().GetWidth();
  m_h = GetParent()->GetClientSize().GetHeight();
  m_ptrOriginX = 0.5*m_w;
  m_ptrOriginY = 0.75*m_h;
  m_dialRadius = 0.35*m_w/sin(m_ptrMaxAngle*pi/180.0);
  m_pointerWidth = m_dialRadius/50;
  m_dotLeftX = 0.2*m_w;
  m_dotRightX = 0.8*m_w;
  m_dotY = 0.1*m_w;
  m_dotRadius = 0.025*m_w;
  fontUnits = wxFont(0.025*m_h,
                     wxFONTFAMILY_SWISS,
                     wxFONTSTYLE_NORMAL,
                     wxFONTWEIGHT_NORMAL);
  fontTicks = wxFont(0.025*m_h,
                     wxFONTFAMILY_SWISS,
                     wxFONTSTYLE_NORMAL,
                     wxFONTWEIGHT_NORMAL);
  fontName = wxFont(0.05*m_h,
                    wxFONTFAMILY_SWISS,
                    wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_NORMAL);
  fontFrequency = wxFont(0.05*m_h,
                         wxFONTFAMILY_SWISS,
                         wxFONTSTYLE_NORMAL,
                         wxFONTWEIGHT_NORMAL);
  fontPitch = wxFont(0.035*m_h,
                     wxFONTFAMILY_SWISS,
                     wxFONTSTYLE_NORMAL,
                     wxFONTWEIGHT_NORMAL);
}

void guiMeter::DrawGuiDial(wxMemoryDC &guiDialDC)
{
  // Clear bitmap
  wxPen pen (*wxWHITE, 1);
  guiDialDC.SetBrush(*wxWHITE_BRUSH);
  guiDialDC.DrawRectangle(0, 0, m_w, m_h);

  // Set the pen
  pen.SetColour(*wxBLACK);
  guiDialDC.SetPen(pen);

  // Draw Units
  guiDialDC.SetFont(fontUnits);
  wxCoord unitsW, unitsH;
  guiDialDC.GetTextExtent(m_units, &unitsW, &unitsH);
  int unitsStartX = m_ptrOriginX - (0.5*unitsW);
  int unitsStartY = m_ptrOriginY - (0.9*m_dialRadius) + (0.5*unitsH);
  guiDialDC.DrawText(m_units, unitsStartX, unitsStartY);

  // Set the ticks font
  guiDialDC.SetFont(fontTicks);
  guiDialDC.SetTextForeground(*wxBLACK);
  guiDialDC.SetBackgroundMode(wxTRANSPARENT);

  // Draw tick marks
  int tickMajorAngle = 20;
  int tickMediumAngle = 10;
  int tickMinorAngle = 2;
  int posText = 1.075*m_dialRadius;
  int lenMajorTick = 1.05*m_dialRadius;
  int lenMediumTick = 1.035*m_dialRadius;
  int lenMinorTick = 1.02*m_dialRadius;
  // Pointer angle from vertical
  int angle; 

  // Draw tick marks
  for(angle=-m_ptrMaxAngle;    // Left hand side
      angle<=m_ptrMaxAngle;    // Right hand side
      angle+=tickMinorAngle)
    {
      float sinAngle = sin(angle*pi/180);
      float cosAngle = cos(angle*pi/180);

      int startX = m_ptrOriginX+(sinAngle*m_dialRadius);
      int startY = m_ptrOriginY-(cosAngle*m_dialRadius);

      float tickRadius = 
        (angle%tickMajorAngle == 0 ? lenMajorTick : 
         (angle%tickMediumAngle == 0 ? lenMediumTick : lenMinorTick));

      // DC origin is at top left corner
      int endX = m_ptrOriginX+(sinAngle*tickRadius);
      int endY = m_ptrOriginY-(cosAngle*tickRadius);

      guiDialDC.DrawLine(startX, startY, endX, endY);
    }

  // Draw values
  for(angle=-m_ptrMaxAngle; angle<=m_ptrMaxAngle; angle+=tickMajorAngle)
    {
      float sinAngle = sin(angle*pi/180);
      float cosAngle = cos(angle*pi/180);

      wxString text;
      text.Printf(wxT("%d"), angle*m_ptrMaxError/m_ptrMaxAngle);

      int textX = m_ptrOriginX+(sinAngle*posText);
      int textY = m_ptrOriginY-(cosAngle*posText);

      wxCoord textW, textH;
      guiDialDC.GetTextExtent(text, &textW, &textH);

      // Text and DC origins are at top left corner
      int textStartX = textX - (0.5*textW*cosAngle) + (textH*sinAngle);
      int textStartY = textY - (0.5*textW*sinAngle) - (textH*cosAngle);

      // WindowsCE seems to have a problem drawing rotated text
      // with a negative angle. To avoid this keep the angle in
      // the range 0 to 360
      int drawnAngle = (angle<0 ? -angle : 360-angle);
      guiDialDC.DrawRotatedText(text, textStartX, textStartY, drawnAngle);
    }
}

void guiMeter::DrawGuiUpdate(wxAutoBufferedPaintDC &guiMeterDC)
{
  // Set the pen
  wxPen pen (*wxBLACK, 1);
  guiMeterDC.SetPen(pen);

  // Set colour
  guiMeterDC.SetTextForeground(*wxBLACK);
  guiMeterDC.SetBackgroundMode(wxTRANSPARENT);

  // Name
  wxString blank = wxString("    ");
  wxString textName(blank);
  if ( m_voiced )
    {
      textName = m_name;
    }
  guiMeterDC.SetFont(fontName);
  wxCoord nameW, nameH;
  guiMeterDC.GetTextExtent(textName, &nameW, &nameH);
  int nameStartX = (0.125*m_w) - (0.5*nameW) ;
  int nameStartY = (0.8*m_h) - (0.5*nameH);
  guiMeterDC.DrawText(textName, nameStartX, nameStartY);
  
  // Frequency
  wxString textFrequency(blank);
  if ( m_voiced )
    {
      textFrequency.Printf(wxT("%3.1f"), m_frequency);
    }
  guiMeterDC.SetFont(fontFrequency);
  wxCoord frequencyW, frequencyH;
  guiMeterDC.GetTextExtent(textFrequency, &frequencyW, &frequencyH);
  int frequencyStartX = (0.8*m_w) - (0.5*frequencyW) ;
  int frequencyStartY = (0.8*m_h) - (0.5*frequencyH);
  guiMeterDC.DrawText(textFrequency, frequencyStartX, frequencyStartY);

  // Pitch
  wxString textPitch(wxT("Not Voiced"));
  if ( m_voiced )
    {
      textPitch.Printf(wxT("%4.1f"), m_pitch);
    }
  guiMeterDC.SetFont(fontPitch);
  wxCoord pitchW, pitchH;
  guiMeterDC.GetTextExtent(textPitch, &pitchW, &pitchH);
  int pitchStartX = (0.5*m_w) - (0.5*pitchW) ;
  int pitchStartY = (0.9*m_h) - (0.5*pitchH);
  guiMeterDC.DrawText(textPitch, pitchStartX, pitchStartY);

  // Pointer
  if (m_voiced)
    {
      int angle = (m_pitchError*m_ptrMaxAngle)/m_ptrMaxError;
      angle = (angle >  m_ptrOffScaleAngle ?  m_ptrOffScaleAngle : angle);
      angle = (angle < -m_ptrOffScaleAngle ? -m_ptrOffScaleAngle : angle);
      int ptrLength = 0.975*m_dialRadius;
      int ptrEndX = m_ptrOriginX + (ptrLength*sin(angle*pi/180));
      int ptrEndY = m_ptrOriginY - (ptrLength*cos(angle*pi/180));
      pen.SetWidth(m_pointerWidth);
      guiMeterDC.SetPen(pen);
      guiMeterDC.DrawLine(m_ptrOriginX, m_ptrOriginY, ptrEndX, ptrEndY);
    }

  // Indicator dots
  pen.SetWidth(1);
  guiMeterDC.SetPen(pen);

  if (m_voiced) {
    int redBelow = -m_ptrMaxError/3;
    int greenAbove = redBelow;
    int greenBelow = -redBelow;
    int redAbove = -redBelow;
    
    if (m_pitchError<redBelow) {
      guiMeterDC.SetBrush(*wxRED_BRUSH);
      guiMeterDC.DrawCircle(m_dotLeftX, m_dotY, m_dotRadius);
      guiMeterDC.SetBrush(*wxWHITE_BRUSH);
      guiMeterDC.DrawCircle(m_dotRightX, m_dotY, m_dotRadius);
    } else if (m_pitchError>greenAbove && m_pitchError<greenBelow) {
      guiMeterDC.SetBrush(*wxGREEN_BRUSH);
      guiMeterDC.DrawCircle(m_dotLeftX, m_dotY, m_dotRadius);
      guiMeterDC.DrawCircle(m_dotRightX, m_dotY, m_dotRadius);
    } else if (m_pitchError>redAbove) {
      guiMeterDC.SetBrush(*wxWHITE_BRUSH);
      guiMeterDC.DrawCircle(m_dotLeftX, m_dotY, m_dotRadius);
      guiMeterDC.SetBrush(*wxRED_BRUSH);
      guiMeterDC.DrawCircle(m_dotRightX, m_dotY, m_dotRadius);
    }
  } else {
    guiMeterDC.SetBrush(*wxWHITE_BRUSH);
    guiMeterDC.DrawCircle(m_dotLeftX, m_dotY, m_dotRadius);
    guiMeterDC.DrawCircle(m_dotRightX, m_dotY, m_dotRadius);
  }
}
