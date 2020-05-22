//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Shared/global.h"
#include "../Shared/Utility.h"



#include "MainRunPage.h"

MainRunPage::MainRunPage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{

    //Step 1: We need to get the usr_pagesId with the pageName field 'MainPage', this is the starting point.
    //Step 2: With the MainPage index, we need to load all forms from usr_forms having the usr_pageId of the MainPage.
    //
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

    //If we are running the engine, there is no parent for this frame.
    if(!Settings.RUN_ENGINE_ENABLED){
        MyEvent my_event( this );
        my_event.m_bMainRunPageWasDestroyed=true;
        GetParent()->ProcessWindowEvent( my_event );
    }

    return bResult;
}