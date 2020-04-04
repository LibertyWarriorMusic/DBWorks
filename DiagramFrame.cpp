//
// Created by Nicholas Zounis on 3/4/20.
//

#include <wx/wxprec.h>
#include<wx/wx.h>
#include "MyEvent.h"
#include "global.h"
#include "Utility.h"

#include "DiagramFrame.h"


wxBEGIN_EVENT_TABLE(DiagramFrame, wxFrame)

wxEND_EVENT_TABLE()

DiagramFrame::DiagramFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );
    m_StatusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
    m_StatusBar = nullptr;
}

DiagramFrame::~DiagramFrame(){

}

wxBoxSizer* DiagramFrame::GetFrameSizer()
{
    return m_MainFormSizer;

}
void DiagramFrame::AttachPanel(wxPanel *panel)
{
    panel->SetParent(this);
    m_MainFormSizer->Add( panel, 0, wxGROW, 0);
    panel->Layout();
    panel->Show();
}

//We can send a message to the parent that this window is destroyed.
bool DiagramFrame::Destroy()
{
    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}