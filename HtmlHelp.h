//
// Created by Nicholas Zounis on 16/3/20.
//

#ifndef DBWORKS_HTMLHELP_H
#define DBWORKS_HTMLHELP_H
#include<wx/wxhtml.h>

class HtmlHelp : public wxFrame{

private:
    wxBoxSizer* m_MainFormSizer;
    wxHtmlWindow *m_HtmlWin;
public:
    explicit HtmlHelp( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);

    ~HtmlHelp() override;

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;

    void SetPage(const int &DocumentID); // Loads document from database
    void SetPage(const wxString &Document); // Document is passed as string.

};


#endif //DBWORKS_HTMLHELP_H
