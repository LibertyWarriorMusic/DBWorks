//
// Created by Nicholas Zounis on 26/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/control.h>

#include "ListBoxManager.h"


BEGIN_EVENT_TABLE(ListBoxManager, wxPanel)

                EVT_MOTION(ListBoxManager::mouseMoved)
                EVT_LEFT_DOWN(ListBoxManager::mouseDown)
                EVT_LEFT_UP(ListBoxManager::mouseReleased)
                EVT_RIGHT_DOWN(ListBoxManager::rightClick)
                EVT_LEAVE_WINDOW(ListBoxManager::mouseLeftWindow)
                EVT_KEY_DOWN(ListBoxManager::keyPressed)
                EVT_KEY_UP(ListBoxManager::keyReleased)
                EVT_MOUSEWHEEL(ListBoxManager::mouseWheelMoved)

                // catch paint events
                EVT_PAINT(ListBoxManager::paintEvent)

END_EVENT_TABLE()


ListBoxManager::ListBoxManager(wxFrame* parent, wxString text) :
        wxWindow(parent, wxID_ANY)
{

    SetMinSize( wxSize(buttonWidth, buttonHeight) );
    m_sText = text;
    m_bPressedDown = false;



}


ListBoxManager::ListBoxManager(wxDialog* parent, wxString text) :
        wxWindow(parent, wxID_ANY)
{
    SetMinSize( wxSize(buttonWidth, buttonHeight) );
    this->m_sText = text;
    m_bPressedDown = false;


}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void ListBoxManager::paintEvent(wxPaintEvent & evt)
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


void ListBoxManager::paintNow()
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


void ListBoxManager::render(wxDC&  dc)
{
    if (m_bPressedDown)
        dc.SetBrush( *wxRED_BRUSH );
    else
        dc.SetBrush( *wxGREY_BRUSH );

    dc.DrawRectangle( 0, 0, buttonWidth, buttonHeight );
    dc.DrawText( m_sText, 1, 1 );

}

void ListBoxManager::mouseDown(wxMouseEvent& event)
{
    m_bPressedDown = true;
    paintNow();
}
void ListBoxManager::mouseReleased(wxMouseEvent& event)
{
    m_bPressedDown = false;
    paintNow();

    //wxMessageBox( wxT("You pressed a custom button") );
}
void ListBoxManager::mouseLeftWindow(wxMouseEvent& event)
{
    if (m_bPressedDown)
    {
        m_bPressedDown = false;
        paintNow();
    }
}

// currently unused events
void ListBoxManager::mouseMoved(wxMouseEvent& event) {

}
void ListBoxManager::mouseWheelMoved(wxMouseEvent& event) {

}
void ListBoxManager::rightClick(wxMouseEvent& event) {

}
void ListBoxManager::keyPressed(wxKeyEvent& event) {
    wxChar uc = event.GetUnicodeKey();
    if ( uc != WXK_NONE )
    {
        // It's a "normal" character. Notice that this includes
        // control characters in 1..31 range, e.g. WXK_RETURN or
        // WXK_BACK, so check for them explicitly.
        if ( uc >= 32 )
        {
            wxLogMessage("You pressed '%c'", uc);
        }
        else
        {
            // It's a control character
            //...
        }
    }
    else // No Unicode equivalent.
    {
        // It's a special key, deal with all the known ones:
        switch ( event.GetKeyCode() )
        {
            case WXK_LEFT:
            case WXK_RIGHT:
                //... move cursor ...
                break;
            case WXK_F1:
                //... give help ...
                break;
        }
    }
}
void ListBoxManager::keyReleased(wxKeyEvent& event) {
    wxChar uc = event.GetUnicodeKey();
    if ( uc != WXK_NONE )
    {
        // It's a "normal" character. Notice that this includes
        // control characters in 1..31 range, e.g. WXK_RETURN or
        // WXK_BACK, so check for them explicitly.
        if ( uc >= 32 )
        {
            wxLogMessage("You pressed '%c'", uc);
        }
        else
        {
            // It's a control character
            //...
        }
    }
    else // No Unicode equivalent.
    {
        // It's a special key, deal with all the known ones:
        switch ( event.GetKeyCode() )
        {
            case WXK_LEFT:
            case WXK_RIGHT:
                //... move cursor ...
                break;
            case WXK_F1:
                //... give help ...
                break;
        }
    }
}