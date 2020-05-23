//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../HtmlHelp.h"
#include "../MyEvent.h"
#include "../Shared/global.h"
#include "../Shared/Utility.h"

#include "DesignPagePanel.h"
#include "DesignPage.h"

enum {
    ID_HELP= wxID_HIGHEST + 10200,
    ID_TOOL_ADD
};

wxBEGIN_EVENT_TABLE(DesignPage, wxFrame)
                EVT_TOOL(ID_HELP, DesignPage::OnbHelp)
                EVT_TOOL(ID_TOOL_ADD, DesignPage::OnbAddItem)
                EVT_MYEVENT(DesignPage::OnMyEvent)
wxEND_EVENT_TABLE()

DesignPage::DesignPage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DiagramFrame( parent, id, title, pos, size, style )
{
    m_pToolbar = nullptr;
    m_pStatusBar = nullptr;
    m_HtmlWin = nullptr;
    m_pDesignPageDiagramPanel = nullptr;

    m_pDesignPageDiagramPanel = new DesignPagePanel(this); //This are our drawing functions.
    AttachPanel(m_pDesignPageDiagramPanel);


    //m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

    //wxBoxSizer *gSizer1 = new wxBoxSizer( wxHORIZONTAL );

   // wxStaticText *txt = new wxStaticText( this, wxID_ANY, "Design Page", wxDefaultPosition, wxDefaultSize, 0 );

    //gSizer1->Add( txt, 0, wxEXPAND, 0);

   // m_MainFormSizer->Add( gSizer1, 0, wxCenter, 5 );
    //Create a HTML display for the help file

    wxInitAllImageHandlers(); //You need to call this or the images will not load.
    // Get the path to the images

    if(m_pToolbar == nullptr)
        m_pToolbar = CreateToolBar();

    wxBitmap BitMap;
    
/*
    m_pComboSelectCtl = new wxComboBox( m_pToolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(230,25), 0,0,wxCB_READONLY);
    m_pComboSelectCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( DesignPage::OnSelectCtlChange ), nullptr, this );
    m_pComboSelectCtl->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( DesignPage::OnSelectCtlDropDown ), nullptr, this );


    Utility::LoadComboFromDatabaseTableByName(Settings.sDatabase,m_pComboSelectCtl,"usr_forms","formName");
    m_pComboSelectCtl->Select(0);
    m_pToolbar->AddControl(m_pComboSelectCtl);



    Utility::LoadBitmap(BitMap,"add.png");
    m_pToolbar->AddTool(ID_TOOL_ADD, wxT(""), BitMap, wxT("Add form to the page."));
*/
    Utility::LoadBitmap(BitMap,"help.png");
    m_pToolbar->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));

    m_pToolbar->Realize();
    SetToolBar(m_pToolbar);
}

DesignPage::~DesignPage(){

    if(m_pDesignPageDiagramPanel!= nullptr)
        m_pDesignPageDiagramPanel->Destroy();
}

void DesignPage::LoadFormObjects()
{
    m_pDesignPageDiagramPanel->LoadFormObjects();
    this->SetSize(800,700);
    this->Show();
}

void DesignPage::SetPageID(wxString sPageId)
{
    m_pDesignPageDiagramPanel->SetPageID(sPageId);
}

void DesignPage::OnSelectCtlDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    int iOldComboIndex = combo->GetSelection();
}

void DesignPage::OnSelectCtlChange( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString sCtlType = combo->GetStringSelection();

    //combo->SetSelection(iOldComboIndex); // Restore the old selection because the database doesn't exit.
}

void DesignPage::OnbAddItem( wxCommandEvent& event )
{
    wxString sformName = m_pComboSelectCtl->GetStringSelection();
    if(!sformName.IsEmpty()){

        wxArrayString sArray;
        Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_forms", "formName", sformName);
        if(sArray.GetCount()==1){
            wxString formId = sArray[0];
            AddDrawObject(formId);
        }
    }
    else{
        wxLogMessage("Select a form first.");
    }
}


void DesignPage::AddDrawObject( const wxString& sFormID)
{
    m_pDesignPageDiagramPanel->AddFormObject( sFormID);
}

//We can send a message to the parent that this window is destroyed.
bool DesignPage::Destroy()
{
    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bDesignPageWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void DesignPage::OnbHelp( wxCommandEvent& event )
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(3); //
    m_HtmlWin->Show(true);

}

void DesignPage::OnMyEvent(MyEvent& event )
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