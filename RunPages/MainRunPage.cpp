//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Utility.h"



#include "MainRunPage.h"

MainRunPage::MainRunPage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *gSizer1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *txt = new wxStaticText( this, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize, 0 );

    gSizer1->Add( txt, 0, wxEXPAND, 0);

    m_MainFormSizer->Add( gSizer1, 0, wxCenter, 5 );
    //Create a HTML display for the help file

}

MainRunPage::~MainRunPage(){

}

//We can send a message to the parent that this window is destroyed.
bool MainRunPage::Destroy()
{
    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bMainRunPageWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}