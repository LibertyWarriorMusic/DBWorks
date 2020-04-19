//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../HtmlHelp.h"
#include "../MyEvent.h"
#include "../Utility.h"

#include "TemplateFrame.h"

enum {
    ID_HELP= wxID_HIGHEST + 10100,
};

wxBEGIN_EVENT_TABLE(TemplateFrame, wxFrame)
                EVT_TOOL(ID_HELP, TemplateFrame::OnbHelp)
                EVT_MYEVENT(TemplateFrame::OnMyEvent)
wxEND_EVENT_TABLE()

TemplateFrame::TemplateFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_sPageId="";
    m_Toolbar = nullptr;
    m_StatusBar = nullptr;
    m_HtmlWin = nullptr;

    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *gSizer1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *txt = new wxStaticText( this, wxID_ANY, "Design Page", wxDefaultPosition, wxDefaultSize, 0 );

    gSizer1->Add( txt, 0, wxEXPAND, 0);

    m_MainFormSizer->Add( gSizer1, 0, wxCenter, 5 );
    //Create a HTML display for the help file

    wxInitAllImageHandlers(); //You need to call this or the images will not load.
    // Get the path to the images

    if(m_Toolbar == nullptr)
        m_Toolbar = CreateToolBar();

    m_StatusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

    wxBitmap BitMap;

    Utility::LoadBitmap(BitMap,"help.png");
    m_Toolbar->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));

    m_Toolbar->Realize();
    SetToolBar(m_Toolbar);
}

TemplateFrame::~TemplateFrame(){

}

void TemplateFrame::SetPageID(wxString sPageId)
{
    m_sPageId=sPageId;
}

//We can send a message to the parent that this window is destroyed.
bool TemplateFrame::Destroy()
{
    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bDesignPageWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void TemplateFrame::OnbHelp( wxCommandEvent& event )
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(3); //
    m_HtmlWin->Show(true);

}

void TemplateFrame::OnMyEvent(MyEvent& event )
{
    // wxString msg;
    // msg << event.GetEventType();
    // wxMessageBox( msg);
    //NOTE: We get here if resizing or right menu clicking, so we need to create a flag to indicated which.
    if(event.m_bHelpFrameWasDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }
    else if(event.m_bRefreshDatabase){


    }
}