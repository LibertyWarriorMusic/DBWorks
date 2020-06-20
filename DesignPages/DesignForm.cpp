//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../HtmlHelp.h"
#include "../MyEvent.h"
#include "../Shared/global.h"
#include "../Shared/Utility.h"
#include "../Dialog/MultiTextDlg.h"
#include "../Generic/GenericQueryGrid.h"

#include "../RuntimeEngine/RunFormFrame.h"
#include "DesignFormPanel.h"
#include "DesignForm.h"

enum {
    ID_HELP= wxID_HIGHEST + 10100,
    ID_TOOL_ADD,
    ID_RUN_FORM,
    ID_EDIT_QUERY
};

wxBEGIN_EVENT_TABLE(DesignForm, wxFrame)
                EVT_TOOL(ID_HELP, DesignForm::OnbHelp)
                EVT_TOOL(ID_TOOL_ADD, DesignForm::OnbAddItem)
                EVT_TOOL(ID_EDIT_QUERY, DesignForm::OnEditQuery)
                EVT_TOOL(ID_RUN_FORM, DesignForm::OnRunForm)
                EVT_MYEVENT(DesignForm::OnMyEvent)
                EVT_SIZE(DesignForm::OnSizeForm)
wxEND_EVENT_TABLE()

void DesignForm::OnRunForm( wxCommandEvent& event ) {
//NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
// be pointing to garbage memory and the program will crash if you try and call Destroy().

    if (m_pRunFormFrame != nullptr)
        m_pRunFormFrame->Destroy();

    m_pRunFormFrame = new RunFormFrame((wxFrame *) this, -1, "Run Form", wxDefaultPosition, wxDefaultSize,
                                     wxDEFAULT_FRAME_STYLE);

    m_pRunFormFrame->SetFormID(m_sFormId); // The formId is used to load the form definition from the database.
    m_pRunFormFrame->Create(m_sBuildQuery);
    m_pRunFormFrame->Show(true);
}



DesignForm::DesignForm( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DiagramFrame( parent, id, title, pos, size, style )
{
    m_sFormId = "";
    m_sBuildQuery = "";
    m_usr_queriesId="";
    m_pToolbar = nullptr;
    m_pStatusBar = nullptr;
    m_pHtmlWin = nullptr;
    m_pQueryGrid = nullptr;
    m_pRunFormFrame = nullptr;
    m_pDesignFormDiagramPanel= nullptr;

    iOldComboIndex=0;

    m_pDesignFormDiagramPanel = new DesignFormPanel(this); //This are our drawing functions.
    AttachPanel(m_pDesignFormDiagramPanel);

   // m_MainFormSizer = new wxBoxSizer( wxVERTICAL );
   // wxBoxSizer *gSizer1 = new wxBoxSizer( wxHORIZONTAL );
   // wxStaticText *txt = new wxStaticText( this, wxID_ANY, "Design Form", wxDefaultPosition, wxDefaultSize, 0 );
   // gSizer1->Add( txt, 0, wxEXPAND, 0);
   // m_MainFormSizer->Add( gSizer1, 0, wxCenter, 5 );

    wxInitAllImageHandlers(); //You need to call this or the images will not load.
    // Get the path to the images

    if(m_pToolbar == nullptr)
        m_pToolbar = CreateToolBar();
    wxBitmap BitMap;

    Utility::LoadBitmap(BitMap,"help.png");
    m_pToolbar->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));

    m_pComboSelectCtl = new wxComboBox( m_pToolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(230,25), 0,0,wxCB_READONLY);
    m_pComboSelectCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( DesignForm::OnSelectCtlChange ), nullptr, this );
    m_pComboSelectCtl->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( DesignForm::OnSelectCtlDropDown ), nullptr, this );


    Utility::LoadComboFromDatabaseTableByName(Settings.sDatabase,m_pComboSelectCtl,"usr_control_types","typeName");
    m_pComboSelectCtl->Select(0);
    m_pToolbar->AddControl(m_pComboSelectCtl);

    Utility::LoadBitmap(BitMap,"add.png");
    m_pToolbar->AddTool(ID_TOOL_ADD, wxT(""), BitMap, wxT("Add control to the page."));

    Utility::LoadBitmap(BitMap,"formQueries.png");
    m_pToolbar->AddTool(ID_EDIT_QUERY, wxT(""), BitMap, wxT("Edit the form query associated with this page."));

    Utility::LoadBitmap(BitMap,"run.png");
    m_pToolbar->AddTool(ID_RUN_FORM, wxT(""), BitMap, wxT("Run form."));

    m_pToolbar->Realize();
    SetToolBar(m_pToolbar);
}

DesignForm::~DesignForm()
{
    if(m_pDesignFormDiagramPanel!= nullptr)
        m_pDesignFormDiagramPanel->Destroy();
}

void DesignForm::OnSelectCtlDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    int iOldComboIndex = combo->GetSelection();
}

void DesignForm::OnSelectCtlChange( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString sCtlType = combo->GetStringSelection();

    //combo->SetSelection(iOldComboIndex); // Restore the old selection because the database doesn't exit.
}

//We can send a message to the parent that this window is destroyed.
bool DesignForm::Destroy()
{
    //Before we do anything, save the form size to the database
    if(!m_sFormId.IsEmpty()){
        wxString queryString = "UPDATE usr_forms SET width="+ Utility::IntToString(m_FormSize.x) + ", height=" + Utility::IntToString(m_FormSize.y) + " WHERE usr_formsId="+m_sFormId;
        Utility::ExecuteQuery(queryString);
    }


    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bDesignFormWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void DesignForm::SetFormID(wxString sFormId)
{
    m_sFormId=sFormId;
    if(m_pDesignFormDiagramPanel!= nullptr){

        m_pDesignFormDiagramPanel->SetFormID(sFormId);

        //Load Query String
        wxString Query = "SELECT usr_queries.queryDefinition, usr_forms.usr_formsId, usr_queries.usr_queriesId, usr_forms.width, usr_forms.height";
        Query+= " FROM usr_queries, usr_forms";
        Query+= " WHERE usr_forms.usr_queriesId = usr_queries.usr_queriesId";
        Query+= " AND usr_forms.usr_formsId = "+ m_sFormId +" LIMIT 1";

        ArrayFieldRecord aRecord;
        Utility::LoadArrayFieldRecordFromQuery(aRecord, Query);
        if(aRecord.GetCount()==1){
            m_sBuildQuery = aRecord[0].GetData("queryDefinition");
            m_usr_queriesId = aRecord[0].GetData("usr_queriesId");

            //Set the form size here.
            m_FormSize.x=Utility::StringToInt(aRecord[0].GetData("width"));
            m_FormSize.y=Utility::StringToInt(aRecord[0].GetData("height"));
        }

        m_pDesignFormDiagramPanel->SetQuery(m_sBuildQuery);



    }

}

void DesignForm::OnbHelp( wxCommandEvent& event )
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_pHtmlWin != nullptr)
        m_pHtmlWin->Destroy();

    m_pHtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_pHtmlWin->SetPage(3); //
    m_pHtmlWin->Show(true);
}

void DesignForm::LoadControlObjects()
{
    m_pDesignFormDiagramPanel->LoadControlObjects();
    //this->SetSize(m_pDesignFormDiagramPanel->GetSizeDiagramExtend());
    this->SetSize(m_FormSize.x,m_FormSize.y);
    this->Show();
}

void DesignForm::OnbAddItem( wxCommandEvent& event )
{

    wxString sCtlType = m_pComboSelectCtl->GetStringSelection();
    if(!sCtlType.IsEmpty()){

        wxArrayString sArray;
        Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_control_types", "typeName", sCtlType);
        if(sArray.GetCount()==1){
            wxString controlTypeId = sArray[0];
            AddDrawObject(controlTypeId);
        }
    }
    else{
        wxLogMessage("Select a control first.");
    }
}

void DesignForm::AddDrawObject( const wxString& sTableID)
{
    m_pDesignFormDiagramPanel->AddControlObject( sTableID);
}

void DesignForm::OnEditQuery( wxCommandEvent& event )
{
    MultiTextDlg * pDlg = new MultiTextDlg(this, "Edit Query", "Query");

    pDlg->SetDataValue("ID_TEXT",m_sBuildQuery);
    if(pDlg->ShowModal()==wxOK) {

        m_sBuildQuery = pDlg->GetDataValue("ID_TEXT");
        m_sBuildQuery.Replace("\r","\n");
        m_pDesignFormDiagramPanel->SetQuery(m_sBuildQuery);

        //Update the database;
        wxString Query = " UPDATE usr_queries SET queryDefinition='"+Utility::EscapeQuery(m_sBuildQuery)+"' WHERE usr_queriesId="+m_usr_queriesId;
        Utility::ExecuteQuery(Query,true);
    }
    pDlg->Destroy();
}

void DesignForm::OnMyEvent(MyEvent& event )
{
    // wxString msg;
    // msg << event.GetEventType();
    // wxMessageBox( msg);
    //NOTE: We get here if resizing or right menu clicking, so we need to create a flag to indicated which.
    if(event.m_bHelpFrameWasDestroyed){
        m_pHtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }
    if(event.m_bRunFormWasDestroyed){
        m_pRunFormFrame = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }
    else if(event.m_bRefreshDatabase){


    }
    else if(event.m_bOpenRunQuery){
        RunQuery();
    }
    else if(event.m_bDestroyed){
        m_pQueryGrid = nullptr;
    }

}

void DesignForm::RunQuery() {

    m_pQueryGrid = new GenericQueryGrid((wxFrame *) this, -1, m_sBuildQuery, wxDefaultPosition, wxDefaultSize,
                                        wxDEFAULT_FRAME_STYLE);

    if (m_pQueryGrid != nullptr) {
        m_pQueryGrid->SetSettings(Settings.sDatabase, Settings.sServer, Settings.sDatabaseUser, Settings.sPassword);
        m_pQueryGrid->SetFormQuery(m_sBuildQuery);
        if (m_pQueryGrid->Create())
            m_pQueryGrid->Show(true);
        else {
            m_pQueryGrid->Close();
            m_pQueryGrid = nullptr;
        }
    }
}

void DesignForm::OnSizeForm(wxSizeEvent& event)
{
    OnSize(event);
    m_FormSize = event.GetSize();//Make sure the form size is current and reflects any change. We are going to save it on exit.
}