//
// Created by Nicholas Zounis on 26/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/control.h>

#include "MyTextCtrl.h"


BEGIN_EVENT_TABLE(MyTextCtrl, wxPanel)

                EVT_MOTION(MyTextCtrl::mouseMoved)
                EVT_LEFT_DOWN(MyTextCtrl::mouseDown)
                EVT_LEFT_UP(MyTextCtrl::mouseReleased)
                EVT_RIGHT_DOWN(MyTextCtrl::rightClick)
                EVT_LEAVE_WINDOW(MyTextCtrl::mouseLeftWindow)
                //EVT_KEY_DOWN(MyTextCtrl::keyPressed)
                //EVT_KEY_UP(MyTextCtrl::keyReleased)
                EVT_MOUSEWHEEL(MyTextCtrl::mouseWheelMoved)

                // catch paint events
                EVT_PAINT(MyTextCtrl::paintEvent)

END_EVENT_TABLE()


MyTextCtrl::MyTextCtrl(wxFrame* parent, wxString text) :
        wxTextCtrl(parent, wxID_ANY)
{

    SetMinSize( wxSize(buttonWidth, buttonHeight) );
    SetSize(wxSize(200,100));
    m_sText = text;
    m_bPressedDown = false;

    //WriteText("Hello World");
}



MyTextCtrl::MyTextCtrl(wxDialog* parent, wxString text) :
        wxTextCtrl(parent, wxID_ANY)
{
    SetMinSize( wxSize(buttonWidth, buttonHeight) );

    SetSize(wxSize(200,100));
    m_sText = text;
    m_bPressedDown = false;
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void MyTextCtrl::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */


void MyTextCtrl::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);

}



/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */


void MyTextCtrl::render(wxDC&  dc)
{
    if (m_bPressedDown)
        dc.SetBrush( *wxRED_BRUSH );
    else
        dc.SetBrush( *wxGREY_BRUSH );

    dc.DrawRectangle( 50, 50, buttonWidth, buttonHeight );
    dc.DrawText( m_sText, 50, 50 );
}

void MyTextCtrl::mouseDown(wxMouseEvent& event)
{
    m_bPressedDown = true;
    paintNow();
}
void MyTextCtrl::mouseReleased(wxMouseEvent& event)
{
    m_bPressedDown = false;
    paintNow();

    //wxMessageBox( wxT("You pressed a custom button") );
}
void MyTextCtrl::mouseLeftWindow(wxMouseEvent& event)
{
    if (m_bPressedDown)
    {
        m_bPressedDown = false;
        paintNow();
    }
}

// currently unused events
void MyTextCtrl::mouseMoved(wxMouseEvent& event) {}
void MyTextCtrl::mouseWheelMoved(wxMouseEvent& event) {}
void MyTextCtrl::rightClick(wxMouseEvent& event) {}
void MyTextCtrl::keyPressed(wxKeyEvent& event) {}
void MyTextCtrl::keyReleased(wxKeyEvent& event) {}