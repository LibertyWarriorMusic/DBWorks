//
// Created by Nicholas Zounis on 26/3/20.
//

#ifndef DBWORKS_myButton_H
#define DBWORKS_myButton_H

#include <wx/wx.h>
#include <wx/sizer.h>


//EXPERIMENTAL NOT USED


class myButton : public wxButton
{
private:

    bool m_bPressedDown;
    wxString m_sAction;
    wxString m_sFormId;
    wxString m_sBuildQuery;

public:
    myButton(wxWindow* parent, wxString title);
    void SetAction(wxString sAction);
    wxString GetAction();
    void SetFormId(wxString sId);
    wxString GetFormId();
    void SetBuildQuery(wxString sQuery);
    wxString GetBuildQuery();
DECLARE_EVENT_TABLE()
};



#endif //DBWORKS_myButton_H
