//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_DesignForm_H
#define DBWORKS_DesignForm_H
#include<wx/wxhtml.h>
class HtmlHelp;

class DesignForm : public wxFrame{
    wxDECLARE_EVENT_TABLE();
private:
    wxBoxSizer* m_MainFormSizer;
    wxToolBar * m_Toolbar;
    wxStatusBar* m_StatusBar;

    wxString m_sFormId;
    HtmlHelp *m_HtmlWin;
    void OnMyEvent(MyEvent& event );
    void OnbHelp( wxCommandEvent& event );

public:
    explicit DesignForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);
    ~DesignForm() override;

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;
    void SetFormID(wxString sPageId);
};


#endif //DBWORKS_MainRunForm_H
