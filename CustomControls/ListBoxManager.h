//
// Created by Nicholas Zounis on 26/3/20.
//

#ifndef DBWORKS_ListBoxManager_H
#define DBWORKS_ListBoxManager_H

#include <wx/wx.h>
#include <wx/sizer.h>

class ListBoxManager : public wxWindow

{
private:

    bool m_bPressedDown;
    wxString m_sText;

    static const int buttonWidth = 100;
    static const int buttonHeight = 25;

public:
    ListBoxManager(wxFrame* parent, wxString text);
    ListBoxManager(wxDialog* parent, wxString text);






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



#endif //DBWORKS_ListBoxManager_H
