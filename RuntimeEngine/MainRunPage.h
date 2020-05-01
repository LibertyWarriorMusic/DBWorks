//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_MainRunPage_H
#define DBWORKS_MainRunPage_H
#include<wx/wxhtml.h>

class MainRunPage : public wxFrame{

private:
    wxBoxSizer* m_MainFormSizer;
public:
    explicit MainRunPage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);

    ~MainRunPage() override;

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;

};


#endif //DBWORKS_MainRunPage_H
