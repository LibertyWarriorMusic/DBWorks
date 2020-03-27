//
// Created by Nicholas Zounis on 26/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/control.h>
#include <wx/gauge.h>
#include "myProgressControl.h"


IMPLEMENT_DYNAMIC_CLASS(myProgressControl,wxGauge);

BEGIN_EVENT_TABLE(myProgressControl, wxGauge)
EVT_PAINT(myProgressControl::OnPaint)
END_EVENT_TABLE()


void myProgressControl::Init()
{
   //

   // SetLabel (txt);

    //wxSize sz(150,10);
    //m_ProgressGauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, sz, wxGA_HORIZONTAL );
    wxSize size(300,30);
    SetSize(size);


}
wxSize myProgressControl::DoGetBestSize() const
{
    // Do not return any arbitrary default value...


  //  wxASSERT_MSG( m_widget, wxT("myStaticText::DoGetBestSize called before creation") );


    // There is probably a better way than this, but I don't know it.
    // So I use the const_cast to grab a dc that is only used for metrics.
    wxClientDC dc (const_cast<myProgressControl*> (this));

    wxCoord w = 0;
    wxCoord h = 0;
    dc.GetTextExtent( GetLabel (), &w, &h );

    return wxSize( w, h );
}

void myProgressControl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    //wxGauge::OnPaint(event);
    int val = GetValue();
    int range = GetRange();




    wxPaintDC dc (this);
    wxCoord
            x = 0,
            y = 0,
            width = 0,
            height = 0,
            tw = 0,
            th = 0;

    GetClientSize( &width, &height );

    dc.SetTextForeground (GetForegroundColour ());
    dc.SetTextBackground (GetBackgroundColour ());

    if(val>0){
        dc.DrawText(GetLabel (),5,0);

    }

    // draw a rectangle
     dc.SetBrush(*wxBLUE_BRUSH); // blue filling
     dc.SetPen( wxPen( wxColor(51,104,205), 2 ) ); // 10-pixels-thick pink outline
    //dc.DrawRectangle( 100, 100, 200, 200 );
    dc.DrawRectangle(5,20,val*2,2);

    //dc.DrawText("This is my control",0,0);

    //dc.DrawText( GetLabel (), 0, 0);
}

void myProgressControl::SetLabel( const wxString& label )
{
    wxGauge::SetLabel( label );
    Refresh ();
}

void myProgressControl::SetValue(int val)
{
    //m_ProgressGauge->SetValue(val);
    //wxGauge::SetLabel( label );
    wxGauge::SetValue(val);
    Refresh ();
}