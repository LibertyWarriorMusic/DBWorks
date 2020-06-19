//
// Created by Nicholas Zounis on 26/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include<wx/control.h>
#include "../MyEvent.h"

#include "../Shared/Utility.h"
#include "../Shared/global.h"

#include "RecordSelector.h"


BEGIN_EVENT_TABLE(RecordSelector, wxPanel)

                EVT_MOTION(RecordSelector::mouseMoved)
                EVT_LEFT_DOWN(RecordSelector::mouseDown)
                EVT_LEFT_UP(RecordSelector::mouseReleased)
                EVT_RIGHT_DOWN(RecordSelector::rightClick)
                EVT_LEAVE_WINDOW(RecordSelector::mouseLeftWindow)
                EVT_KEY_DOWN(RecordSelector::keyPressed)
                EVT_KEY_UP(RecordSelector::keyReleased)
                EVT_MOUSEWHEEL(RecordSelector::mouseWheelMoved)

                // catch paint events
                EVT_PAINT(RecordSelector::paintEvent)

END_EVENT_TABLE()

RecordSelector::RecordSelector(wxWindow* parent, wxString text) :
        wxWindow(parent, wxID_ANY)
{

    SetMinSize( wxSize(buttonWidth, buttonHeight) );
    m_sText = text;
    m_bPressedDown = false;

    m_iRecordIndex=0;
}

RecordSelector::RecordSelector(wxFrame* parent, wxString text) :
        wxWindow(parent, wxID_ANY)
{

    SetMinSize( wxSize(buttonWidth, buttonHeight) );
    m_sText = text;
    m_bPressedDown = false;

    m_iRecordIndex=0;
}

void RecordSelector::LoadAllRecordID(wxString sTableName)
{
    Utility::LoadStringArrayWithAllIDSFromTable(sTableName, m_asRecordID);
}

RecordSelector::RecordSelector(wxDialog* parent, wxString text) :
        wxWindow(parent, wxID_ANY)
{
    SetMinSize( wxSize(buttonWidth, buttonHeight) );
    this->m_sText = text;
    m_bPressedDown = false;
}
wxString RecordSelector::GetCurrentRecordID()
{
    if(m_asRecordID.GetCount()>0)
        return m_asRecordID[m_iRecordIndex];

    return "";
}
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void RecordSelector::paintEvent(wxPaintEvent & evt)
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


void RecordSelector::paintNow()
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

void RecordSelector::render(wxDC&  dc)
{
    if (m_bPressedDown)
        dc.SetBrush( *wxRED_BRUSH );
    else
        dc.SetBrush( *wxGREY_BRUSH );

    dc.DrawRectangle( 0, 0, buttonWidth, buttonHeight );
    //dc.DrawText( m_sText, 1, 1 );

    if (m_asRecordID.GetCount()>0){
        dc.DrawText( "Record:" + m_asRecordID[m_iRecordIndex], 1, 1 );
    }
}

void RecordSelector::mouseDown(wxMouseEvent& event)
{
    m_bPressedDown = true;
    paintNow();
}

void RecordSelector::mouseReleased(wxMouseEvent& event)
{
    m_bPressedDown = false;

    m_iRecordIndex++;
    if(m_iRecordIndex==m_asRecordID.GetCount())
        m_iRecordIndex=0;

    paintNow();

    //Send a message to the parent.
    MyEvent my_event( this );
    my_event.m_bButtonClicked=true;
    my_event.m_sTableId = m_asRecordID[m_iRecordIndex]; // Pass the current tableId to the run engine.
    GetParent()->ProcessWindowEvent( my_event );

    //wxMessageBox( wxT("You pressed a custom button") );
}

void RecordSelector::mouseLeftWindow(wxMouseEvent& event)
{
    if (m_bPressedDown)
    {
        m_bPressedDown = false;
        paintNow();
    }
}

// currently unused events
void RecordSelector::mouseMoved(wxMouseEvent& event) {

}
void RecordSelector::mouseWheelMoved(wxMouseEvent& event) {

}
void RecordSelector::rightClick(wxMouseEvent& event) {

}
void RecordSelector::keyPressed(wxKeyEvent& event) {
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
void RecordSelector::keyReleased(wxKeyEvent& event) {
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