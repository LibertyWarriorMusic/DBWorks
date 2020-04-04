//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "MyEvent.h"
#include "HtmlHelp.h"

#include "Utility.h"




HtmlHelp::HtmlHelp( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );
    //Create a HTML display for the help file
    m_HtmlWin = new wxHtmlWindow(this);
    m_HtmlWin->SetRelatedFrame(this, "HTML : %%s");
    //m_HtmlWin->SetRelatedStatusBar(0);
    m_MainFormSizer->Add( m_HtmlWin, 0, wxGROW, 0);
    //m_HtmlWin->SetSize(-1,300);
    m_HtmlWin->Show();

}

HtmlHelp::~HtmlHelp(){

}

//We can send a message to the parent that this window is destroyed.
bool HtmlHelp::Destroy()
{
    bool bResult = wxFrame::Destroy();


    MyEvent my_event( this );
    my_event.m_bHelpFrameWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void HtmlHelp::SetPage(const int &DocumentID){
    m_HtmlWin->SetPage(Utility::PassHTMLDocument(Utility::LoadSystemDocument(DocumentID)));
    m_HtmlWin->Show();
}

void HtmlHelp::SetPage(const wxString &Document){
    m_HtmlWin->SetPage(Document);
    m_HtmlWin->Show();
}
