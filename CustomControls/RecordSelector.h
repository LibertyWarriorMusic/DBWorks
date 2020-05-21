//
// Created by Nicholas Zounis on 26/3/20.
//

#ifndef DBWORKS_RecordSelector_H
#define DBWORKS_RecordSelector_H

#include <wx/wx.h>
#include <wx/sizer.h>

//EXPERIMENTAL NOT USED

class RecordSelector : public wxWindow

{
private:

    bool m_bPressedDown;
    wxString m_sText;

    wxArrayString m_asRecordID; // Holds all the record ID's for the query.

    int m_iRecordIndex;

    static const int buttonWidth = 100;
    static const int buttonHeight = 25;

public:
    RecordSelector(wxFrame* parent, wxString text);
    RecordSelector(wxDialog* parent, wxString text);

    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    void LoadAllRecordID(wxString sTableName);

    wxString GetCurrentRecordID();

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
