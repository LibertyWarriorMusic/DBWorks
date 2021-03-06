//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../HtmlHelp.h"
#include "../MyEvent.h"
#include "../Shared/global.h"
#include "../Shared/Utility.h"
#include "../RuntimeEngine/MainRunPage.h"

#include "DesignPagePanel.h"
#include "DesignPage.h"

enum {
    ID_HELP= wxID_HIGHEST + 10200,
    ID_TOOL_ADD,
    ID_RUN_FORM
};

wxBEGIN_EVENT_TABLE(DesignPage, wxFrame)
                EVT_TOOL(ID_HELP, DesignPage::OnbHelp)
                EVT_TOOL(ID_TOOL_ADD, DesignPage::OnbAddItem)
                EVT_TOOL(ID_RUN_FORM, DesignPage::OnRunPage)
                EVT_MYEVENT(DesignPage::OnMyEvent)
                EVT_SIZE(DesignPage::OnSizePage)
wxEND_EVENT_TABLE()

void DesignPage::OnRunPage( wxCommandEvent& event ) {

    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_pMainRunPage != nullptr)
        m_pMainRunPage->Destroy();

    m_pMainRunPage = new MainRunPage((wxFrame*) this, -1,m_pDesignPageDiagramPanel->GetPageID(), "Your first database", wxDefaultPosition, wxSize(1700,1000), wxDEFAULT_FRAME_STYLE );
    m_pMainRunPage->Show(true);
}

DesignPage::DesignPage( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DiagramFrame( parent, id, title, pos, size, style )
{
    m_pToolbar = nullptr;
    m_pStatusBar = nullptr;
    m_HtmlWin = nullptr;
    m_pDesignPageDiagramPanel = nullptr;
    m_pMainRunPage = nullptr;

    m_pDesignPageDiagramPanel = new DesignPagePanel(this); //This are our drawing functions.
    AttachPanel(m_pDesignPageDiagramPanel);

    wxInitAllImageHandlers(); //You need to call this or the images will not load.
    // Get the path to the images

    if(m_pToolbar == nullptr)
        m_pToolbar = CreateToolBar();

    wxBitmap BitMap;

    Utility::LoadBitmap(BitMap,"help.png");
    m_pToolbar->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));

    Utility::LoadBitmap(BitMap,"run.png");
    m_pToolbar->AddTool(ID_RUN_FORM, wxT(""), BitMap, wxT("Run form."));

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


    wxString pageId = m_pDesignPageDiagramPanel->GetPageID();
    //Load the page size
    //Load Query String
    wxString Query = "SELECT width, height";
    Query+= " FROM usr_pages";
    Query+= " WHERE usr_pagesId = "+ pageId +" LIMIT 1";

    ArrayFieldRecord aRecord;
    Utility::LoadArrayFieldRecordFromQuery(aRecord, Query);
    if(aRecord.GetCount()==1){

        //Set the form size here.
        m_PageSize.x=Utility::StringToInt(aRecord[0].GetData("width"));
        m_PageSize.y=Utility::StringToInt(aRecord[0].GetData("height"));
    }

    this->SetSize(m_PageSize);


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
    wxString pageId = m_pDesignPageDiagramPanel->GetPageID();

    //Before we do anything, save the form size to the database
    if(!pageId.IsEmpty()){
        wxString queryString = "UPDATE usr_pages SET width="+ Utility::IntToString(m_PageSize.x) + ", height=" + Utility::IntToString(m_PageSize.y) + " WHERE usr_pagesId="+pageId;
        Utility::ExecuteQuery(queryString);
    }


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
    else if(event.m_bMainRunPageWasDestroyed){
        m_pMainRunPage = nullptr;
    }
    else if(event.m_bRefreshDatabase){


    }
}

void DesignPage::OnSizePage(wxSizeEvent& event)
{
    OnSize(event);
    m_PageSize = event.GetSize();//Make sure the form size is current and reflects any change. We are going to save it on exit.
}