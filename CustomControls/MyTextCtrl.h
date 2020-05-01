//
// Created by Nicholas Zounis on 26/3/20.
//

#ifndef DBWORKS_MyTextCtl_H
#define DBWORKS_MyTextCtl_H

#include <wx/wx.h>
#include <wx/sizer.h>

class MyTextCtrl : public wxTextCtrl
{
private:

    bool m_bPressedDown;
    wxString m_sText;

    static const int buttonWidth = 200;
    static const int buttonHeight = 100;

public:
    MyTextCtrl(wxFrame* parent, wxString text);
    MyTextCtrl(wxDialog* parent, wxString text);



    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    void render(wxDC& dc);

    // some useful events
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

DECLARE_EVENT_TABLE()
};



#endif //DBWORKS_MyTextCtl_H
