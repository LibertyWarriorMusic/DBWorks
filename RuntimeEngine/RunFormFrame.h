//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_RunFormFrame_H
#define DBWORKS_RunFormFrame_H
#include<wx/wxhtml.h>

class RunForm;

class RunFormFrame : public wxFrame{

private:
    wxDECLARE_EVENT_TABLE();

public:
    explicit RunFormFrame( wxFrame* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);


    void OnMyEvent(MyEvent& event);

    ~RunFormFrame() override;

    RunForm * m_pRunForm;
    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;
    wxString GetQuery(wxString sFormId="");
    void SetFormID(wxString Id);
    void Create(wxString sQuery);
};

#endif //DBWORKS_RunFormFrame_H
