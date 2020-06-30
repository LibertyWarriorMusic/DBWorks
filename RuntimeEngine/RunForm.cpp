//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include <wx/hyperlink.h>
#include <wx/datectrl.h>

#include "../CustomControls/myButton.h"
#include "../CustomControls/RecordSelector.h"
#include "../CustomControls/RecordScrollDisplay.h"
#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "../Shared/global.h"
#include <mysql.h>
#include <mysql++.h>


#include "RunForm.h"

using namespace mysqlpp;
wxBEGIN_EVENT_TABLE(RunForm, wxWindow)
                EVT_MYEVENT(RunForm::OnMyEvent)
wxEND_EVENT_TABLE()


RunForm::RunForm( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxWindow( parent, id, pos, size, style )
{
    m_CalculatedHeightWindow=0;
    m_sOldSelectionText="";
    wxString m_sFormId=""; //Set for testing
    m_sCurrentId="";
    m_bLoadFields=false;
    m_pRecordScrollDisplay = nullptr;

    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );
}

void RunForm::Create(wxString sQuery)
{
    SetQuery(sQuery);
    m_bLoadFields=false;
    LoadControlObjects();
    RenderAllControls();

    if(m_bLoadFields)
        LoadFields(m_sCurrentId);

    SetSize(m_FormSize);// Set the form Size.
}

wxString RunForm::GetFormID()
{
    return m_sFormId; //This will return the current form ID from the selection control
}
wxSize RunForm::GetFormSize()
{
    return m_FormSize;
}
void RunForm::SetFormID(wxString Id)
{
    m_sFormId = Id;

    //Once we have the form ID, we can load the width and height of the form

    //Load Query String
    wxString Query = "SELECT width, height";
    Query+= " FROM usr_forms";
    Query+= " WHERE usr_formsId = "+ m_sFormId +" LIMIT 1";

    ArrayFieldRecord aRecord;
    Utility::LoadArrayFieldRecordFromQuery(aRecord, Query);
    if(aRecord.GetCount()==1){

        //Set the form size here.
        m_FormSize.x=Utility::StringToInt(aRecord[0].GetData("width"));
        m_FormSize.y=Utility::StringToInt(aRecord[0].GetData("height"));
    }
}

void RunForm::SetFormMode(int mode)
{
    m_iRunMode = mode;
}

int RunForm::GetFormMode()
{
    return m_iRunMode;
}

void RunForm::SetQuery(wxString sQuery)
{
    m_sBuildQuery = sQuery;
    //Extract all the table names from the select query.
    Utility::LoadStringArrayWithTableNamesFromSelectQuery(m_sBuildQuery, m_TableList);
}

wxString RunForm::GetQuery(wxString sFormId)
{

    if(sFormId.IsEmpty())
        return m_sBuildQuery;

    //If we have a form ID, get the query from the database.
    m_sFormId=sFormId;

    //Load Query String
    wxString Query = "SELECT usr_queries.queryDefinition, usr_forms.usr_formsId, usr_queries.usr_queriesId";
    Query+= " FROM usr_queries, usr_forms";
    Query+= " WHERE usr_forms.usr_queriesId = usr_queries.usr_queriesId";
    Query+= " AND usr_forms.usr_formsId = "+ m_sFormId +" LIMIT 1";

    ArrayFieldRecord aRecord;
    Utility::LoadArrayFieldRecordFromQuery(aRecord, Query);
    if(aRecord.GetCount()==1)
        m_sBuildQuery = aRecord[0].GetData("queryDefinition");

    return m_sBuildQuery;
}

//Loads all the controls from the database and creates the drawing
void RunForm::LoadControlObjects()
{
    // If we have existing object, clear the list.
    //if(m_ControlList.GetCount()>0)
        //m_ControlList.Clear();

        //NOTE this query builds the form, that the user defined form query will fill.
    wxString QueryString = "SELECT usr_control_types.usr_control_typesId, usr_control_types.typeName, usr_controls.usr_controlsId, usr_controls.table_data ";
    QueryString += " FROM usr_control_types, usr_controls ";
    QueryString += " WHERE usr_control_types.usr_control_typesId = usr_controls.usr_control_typesId";
    QueryString += " AND usr_controls.usr_formsId = "+GetFormID();

    ArrayFieldRecord saControl;
    Utility::LoadArrayFieldRecordFromQuery(saControl, QueryString);

    int count = saControl.GetCount();
    if(saControl.GetCount()>0){
        for(int index=0;index<count;index++){
            wxString sData="";
            wxString sKey="ObControlShow";

            wxString sControlId = saControl[index].GetData("usr_controlsId");
            wxString sTypeName = saControl[index].GetData("typeName");
            wxString sTypeID = saControl[index].GetData("usr_control_typesId");
            wxString sEntireTableData = saControl[index].GetData("table_data");
            wxString PosX="";
            wxString PosY="";

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,sKey,sData,sEntireTableData);
            Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,"ObControlPositionX",PosX,sEntireTableData);
            Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,"ObControlPositionY",PosY,sEntireTableData);

            if(sData=="yes"){
                //Load the data and create the table object.
                //wxString sTableID = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,saControlId[index]);

                //The table doesn't exist, so add it.
                //ObControl * pCtl = NewCtlObject();
                FieldCtlItem* pCtl = NewFieldDataCtrl();// Creates a new FieldCtlItem in the list and returns a point to that control

                //NOTE: This class is similar to the DialogBaseClass, but working on a frame, rather than a dialog.
                // Also, when creating controls in the dialog class, sIdentifier is used to identify the controls.
                //This the DialogBaseClass, these are public functions and sIdentifier can be set to any value.
                // In order to identify controls, we set sIdentifier = sControlID which is a unique field.
                pCtl->m_sIdentifier = sControlId;

                pCtl->m_sControlId=sControlId;
                pCtl->m_sFormId=GetFormID();
                pCtl->SetTypeName(sTypeName);
                pCtl->m_sTypeId=sTypeID;
                // The position information is used to determine the order in which the cotrols are layed out.
                pCtl->m_rect.x = Utility::StringToInt(PosX);
                pCtl->m_rect.y = Utility::StringToInt(PosY);
                pCtl->m_rect.width = 100;
                pCtl->m_rect.height= 25;

                wxString sLabel="";
                wxString sField="";
                wxString xPos = "";
                wxString yPos = "";
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("ObControlPositionX"),xPos);
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("ObControlPositionY"),yPos);

                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Label"),sLabel);
                pCtl->m_sLabel=sLabel;

                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Short_Description"),sLabel);
                pCtl->m_sDescription=sLabel;

                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Field"),sField);
                //pCtl->m_sField=sField;
                sField.Trim(true);
                sField.Trim(false);
                pCtl->fieldName=sField;


                m_CtrlDataItem.Add(pCtl);
            }
        }
        //RedrawControlObjects(); // Draw them to the screen.
    }
}


FieldCtlItem* RunForm::NewFieldDataCtrl()
{
    FieldCtlItem *item = new FieldCtlItem();
    return item;
}

bool RunForm::GetControlIndexWithLowestYValue(int &index){

    index = wxNOT_FOUND;
    int LowestIndex=wxNOT_FOUND;
    int LowestYPos=0;

    bool bFoundItem=false;
    bool bFirst=true;

    //We need to run through all of them and determine the lowest.
    for(int idx=0; idx < m_CtrlDataItem.GetCount(); idx++){

        if(m_CtrlDataItem[idx].bFound==false){
            int YPos = m_CtrlDataItem[idx].m_rect.y;
            if(bFirst){
                LowestYPos = YPos;
                LowestIndex = idx;
                bFirst=false;
                bFoundItem=true;
                continue;
            }
            bFoundItem=true;

            if (YPos<LowestYPos){
                LowestYPos = YPos;
                LowestIndex = idx;
            }
        }
    }

    index = LowestIndex;

    if(LowestIndex!=wxNOT_FOUND)
        m_CtrlDataItem[LowestIndex].bFound=true;
    else
        return false; //We didn't find anything

    return bFoundItem;
}

//After we add all our control, we call this function to render our constrols to the main sizer so they display
void RunForm::RenderAllControls()
{
    //Clear found flags, just in case and check for a grid display
    for(int idx=0; idx < m_CtrlDataItem.GetCount(); idx++){
        m_CtrlDataItem[idx].bFound=false;
        if(m_CtrlDataItem[idx].iTypeOfControl==CTL_GRID_DISPLAY && m_pRecordScrollDisplay==nullptr){
            wxSize size(GetFormSize().x, GetFormSize().y);
            m_pRecordScrollDisplay = new RecordScrollDisplay(this,wxID_ANY, wxDefaultPosition, size,(unsigned)wxVSCROLL | (unsigned)wxFULL_REPAINT_ON_RESIZE);

            if(m_TableList.GetCount()>0){
                m_pRecordScrollDisplay->LoadAllRecordID(m_TableList[0]);
                m_sCurrentId = m_pRecordScrollDisplay->GetCurrentRecordID();
                int noRecord = Utility::GetNumberOfRecords(m_sBuildQuery);
                m_pRecordScrollDisplay->SetScrollbar(wxVERTICAL,0,1,noRecord);

                m_bLoadFields=true; //Make sure we load the first field.
            }
        }
    }

    int LowestIndex = wxNOT_FOUND;
    while(GetControlIndexWithLowestYValue(LowestIndex)){
        RenderControl(LowestIndex);
    }

    ResizeTitleText(); //Find the maximum width of the title fields and set all controls to that width.
    this->Layout();
    this->Centre( wxBOTH );

    m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
    m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH + 10;

    //this->SetPosition(m_Winpos);
    this->SetSize(WINDOW_WIDTH,m_CalculatedHeightWindow);
    //this->SetLabel(title);
    this->SetSizer( m_MainFormSizer );

    this->Layout();

    //Create the buttons to the dialog
    //Create();
}

void RunForm::RenderControl(int index)
{
    wxBoxSizer* gSizer1;
    wxString sFlags="";
    wxString sTableName="";
    wxString sFieldName="";
    wxString sAction="";
    wxString sControlId="";
    bool bWebLink = false;
    unsigned long style=0;

    wxWindow *pParent = this;

    if(m_pRecordScrollDisplay != nullptr)
        pParent = m_pRecordScrollDisplay;


    sFlags =  m_CtrlDataItem[index].Flags; // Get additional flags for the control
    switch(m_CtrlDataItem[index].iTypeOfControl) {
        case CTL_NULL :

            break;
        case CTL_SPACER :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].SpacerCtl = new wxStaticText( pParent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 );
            //m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            //m_CtrlDataItem[index].TitleCtl->SetSize(500,30);
            //m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].SpacerCtl, 1, wxALL, BORDER_WIDTH);

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            break;

        case CTL_STATIC :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );

            m_CtrlDataItem[index].SpacerCtl = new wxStaticText( pParent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 );
            gSizer1->Add( m_CtrlDataItem[index].SpacerCtl, 0, wxALL, BORDER_WIDTH);

            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            //m_CtrlDataItem[index].TitleCtl->SetSize(500,30);
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 1, wxALL, BORDER_WIDTH);

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            break;
        case CTL_TEXT :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY"))
                style = wxTE_READONLY;
                //The only place we actually want to see the password is when we view the table.
            else if(Utility::HasFlag(m_CtrlDataItem[index].Flags,"PASSWORD") )
                style = wxTE_PASSWORD;

            m_CtrlDataItem[index].textCtl = new wxTextCtrl( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
            m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_PASSWORD :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY"))
                style = wxTE_READONLY;
                //The only place we actually want to see the password is when we view the table.


            style |= wxTE_PASSWORD;

            m_CtrlDataItem[index].textCtl = new wxTextCtrl( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
            m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_MULTI_TEXT :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            //Don't do rich text right now because you copy and past into the field, some special characters might be present that you can't see.
            //I still need to implement parsing for special characters, then we might be able to use this style again.
            //unsigned long style =  wxTE_WORDWRAP | wxTE_RIGHT| wxTE_DONTWRAP | wxTE_MULTILINE | wxTE_RICH | wxHSCROLL;

            style =  wxTE_WORDWRAP | wxTE_RIGHT | wxTE_MULTILINE  | wxHSCROLL;

            if (Utility::HasFlag( m_CtrlDataItem[index].Flags,"READONLY"))
                style |= wxTE_READONLY;

            //m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style);

            m_CtrlDataItem[index].textCtl = new wxTextCtrl( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,TEXTCTLMULTI_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].textCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //This will probably not be used for combos, but kept it hear because it might be used to remember a default value for the control
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += TEXTCTLMULTI_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1, ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION_LINKED_NAME :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            m_asSelectionItemArray.Clear();

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Linked_Table",sTableName);
            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Display_Field",sFieldName);


            m_CtrlDataItem[index].comCtl = new wxComboBox( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style );
            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //Fill the list box with the selection items.
            Utility::LoadStringArrayFromDatabaseTableByName(Settings.sDatabase, m_asSelectionItemArray,sTableName,sFieldName);
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);

            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_SELECTION_LOOKUP_NAME :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            m_asSelectionItemArray.Clear();
            sControlId = m_CtrlDataItem[index].m_sControlId;

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,"Linked_Table",sTableName);
            Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,"Display_Field",sFieldName);

            m_CtrlDataItem[index].comCtl = new wxComboBox( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style );
            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //Fill the list box with the selection items.
            Utility::LoadStringArrayFromDatabaseTableByName(Settings.sDatabase, m_asSelectionItemArray,sTableName,sFieldName);
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);

            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION_ADDITIVE :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            style=0;
            //Create a wxComboCtrl. We need to fill it with the values extracted from

            //Extract all the selection items.
            m_asSelectionItemArray.Clear();
            LoadSelectionItemsFromUsrControlItemsTable(m_CtrlDataItem[index].m_sControlId);

            // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
            // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
            //if (m_sUse=="VIEW" || find != wxNOT_FOUND)
            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY")){
                style |= wxCB_READONLY;
            }

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"DISABLED")){
                m_CtrlDataItem[index].comCtl->Disable();
            }

            m_CtrlDataItem[index].comCtl = new wxComboBox( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);
            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);

            //Make sure you set the default value after you append selection items.
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            //Attache handlers to buttons
            //These events will prevent the replacement of text and add them together.
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( RunForm::OnComboChange ), nullptr, this );
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( RunForm::OnComboDropDown ), nullptr, this );

            m_MainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            style=0;
            //Create a wxComboCtrl. We need to fill it with the values extracted from

            //Extract all the selection items.
            m_asSelectionItemArray.Clear();
            LoadSelectionItemsFromUsrControlItemsTable(m_CtrlDataItem[index].m_sControlId);

            // The issue here is, if we are viewing the text control only and have a value in the control that isn't in the pull down list
            // then that value will not be loaded if we have a read only flag. If we are only viewing the combobox, it's proably better to disable the pull down.
            //if (m_sUse=="VIEW" || find != wxNOT_FOUND)


            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY")){
                style |= wxCB_READONLY;
            }

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"DISABLED")){
                m_CtrlDataItem[index].comCtl->Disable();
            }

            m_CtrlDataItem[index].comCtl = new wxComboBox( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);


            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);


            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);
            // for ( int iIdx=0; iIdx<sSelectionItemArray.GetCount(); iIdx++ )
            //m_CtrlDataItem[index].comCtl->Append(sSelectionItemArray[iIdx]);

            //Make sure you set the default value after you append selection items.
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            //Attache handlers to buttons
            // Connect a closeup event so I can remove the comments.
            
            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_CHECKBOX :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            m_CtrlDataItem[index].pCheckBox = new wxCheckBox( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            gSizer1->Add( m_CtrlDataItem[index].pCheckBox, 0, wxALL, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_DATE :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            style = wxDP_DEFAULT | wxDP_DROPDOWN | wxDP_SHOWCENTURY;
            m_CtrlDataItem[index].datePickerCtl = new wxDatePickerCtrl( pParent, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, style );

            m_CtrlDataItem[index].datePickerCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].datePickerCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );

            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].datePickerCtl->SetValue(Utility::StringToDate(m_CtrlDataItem[index].fieldDefault));
            else if(m_CtrlDataItem[index].fieldType == "DATE") { //Check to see if the type is date, then set with current date.
                m_CtrlDataItem[index].datePickerCtl->SetValue(wxDateTime::Now());
            }

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY"))
                m_CtrlDataItem[index].datePickerCtl->Disable();

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_WEBLINK :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            style = 0;

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY")){
                style = wxTE_READONLY;
                bWebLink=true;
            }

            if(bWebLink){
                //This Hyperlink control needs to have a URL and label, so I just added google, but it gets writen over
                m_CtrlDataItem[index].linkCtl = new wxHyperlinkCtrl( pParent, wxID_ANY,"weblink","https://www.google.com", wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU | wxHL_DEFAULT_STYLE ,wxHyperlinkCtrlNameStr);
                m_CtrlDataItem[index].linkCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                gSizer1->Add( m_CtrlDataItem[index].linkCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
                m_CtrlDataItem[index].linkCtl->SetURL("");
                m_CtrlDataItem[index].linkCtl->SetLabel("");

                if(!m_CtrlDataItem[index].fieldDefault.IsEmpty()){
                    m_CtrlDataItem[index].linkCtl->SetURL(m_CtrlDataItem[index].fieldDefault);
                    m_CtrlDataItem[index].linkCtl->SetLabel(m_CtrlDataItem[index].fieldDefault);
                }
            }else{

                m_CtrlDataItem[index].textCtl = new wxTextCtrl( pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
                m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                gSizer1->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


                if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                    m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);
            }

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

        break;


        case CTL_BUTTON :


            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].m_sDescription, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sDescription);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style = 0;

            m_CtrlDataItem[index].pmyButton = new myButton( pParent, m_CtrlDataItem[index].m_sLabel );
            m_CtrlDataItem[index].pmyButton->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].pmyButton,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );

            m_CtrlDataItem[index].pmyButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunForm::OnButtonClick ), nullptr, this );

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Action",sAction);
            m_CtrlDataItem[index].pmyButton->SetAction(sAction);

            if(sAction=="ACTION_OPEN_FORM"){

                //See if we have a form saved, then select that form.
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"ActionFormToOpen",sAction);
                wxArrayString sArray;
                Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_Forms", "formName", sAction);
                m_CtrlDataItem[index].pmyButton->SetFormId(sArray[0]);

                sArray.Clear();
                Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_Forms", "usr_queriesId", "usr_FormsId", m_CtrlDataItem[index].pmyButton->GetFormId());
                wxString queryID = sArray[0];

                sArray.Clear();
                Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sArray, "usr_queries", "queryDefinition", "usr_queriesId", queryID);
                wxString BuildQuery = sArray[0];

                m_CtrlDataItem[index].pmyButton->SetBuildQuery(BuildQuery);



            }

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

            break;

        case CTL_RECORD_SELECTOR :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( pParent, wxID_ANY, m_CtrlDataItem[index].m_sDescription, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sDescription);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style = 0;

            m_CtrlDataItem[index].pRecordSelector = new RecordSelector( pParent, "Record Selector" );
            m_CtrlDataItem[index].pRecordSelector->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].pRecordSelector,0, wxALL, BORDER_WIDTH );

            if(m_TableList.GetCount()>0){
                m_CtrlDataItem[index].pRecordSelector->LoadAllRecordID(m_TableList[0]);
                    m_sCurrentId = m_CtrlDataItem[index].pRecordSelector->GetCurrentRecordID();
            }

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            m_MainFormSizer->Add( gSizer1,0, 0, BORDER_WIDTH );

            m_bLoadFields=true;

            break;

        default:
            break;
    }
}

void RunForm::LoadSelectionItemsFromUsrControlItemsTable(wxString sControlId){

    ArrayFieldRecord aRecord;
    wxString sQuery = "SELECT usr_control_itemsId, itemData, usr_controlsId FROM usr_control_items WHERE itemType='SELECTION_ITEM' AND usr_controlsId="+sControlId;
    Utility::LoadArrayFieldRecordFromQuery(aRecord,sQuery);

    for (int index=0; index<aRecord.GetCount();index++){
        wxString sData = aRecord[index].GetData("itemData");
        wxString sControlID = aRecord[index].GetData("usr_controlsId");
        wxString sUsrControlItemsId = aRecord[index].GetData("usr_control_itemsId");

        ComboDataItem* comboItem = new ComboDataItem;
        comboItem->tableID = sUsrControlItemsId;
        comboItem->linkedTableID = sControlID;
        comboItem->data=sData;

        //We also want to save the controlID
        m_asSelectionItemArray.Add(comboItem->data);
    }
}

//Search each text label and find the largest lable width.
//Once found, set all labels to that maximum value.
void RunForm::ResizeTitleText()
{
    int num_cols=m_CtrlDataItem.GetCount();
    int MaxWidth=0;
    int WidthOfChar = 0;
    wxString str = "";
    int numChar = 0;
    int widthOfText = 0;

    // First find the maximum label width
    for(int col=0;col<num_cols;col++)
    {
        if( m_CtrlDataItem[col].TitleCtl != nullptr  && m_CtrlDataItem[col].m_sTypeName!="CTL_SPACER"){

            if(m_CtrlDataItem[col].m_sTypeName=="CTL_STATIC"){
                WidthOfChar = m_CtrlDataItem[col].SpacerCtl->GetCharWidth();
                str = m_CtrlDataItem[col].SpacerCtl->GetLabel();
            }else{
                WidthOfChar = m_CtrlDataItem[col].TitleCtl->GetCharWidth();
                str = m_CtrlDataItem[col].TitleCtl->GetLabel();
            }

            numChar = str.Length();
            widthOfText = numChar * WidthOfChar;

            if (widthOfText > MaxWidth)
                MaxWidth = widthOfText;
        }
    }

    for(int col=0;col<num_cols;col++){
        if( m_CtrlDataItem[col].TitleCtl != nullptr && m_CtrlDataItem[col].m_sTypeName!="CTL_SPACER"){

            if(m_CtrlDataItem[col].m_sTypeName=="CTL_STATIC"){
                m_CtrlDataItem[col].SpacerCtl->SetMinSize(wxSize(MaxWidth + 5, CTRL_HEIGHT));
            }else{
                m_CtrlDataItem[col].TitleCtl->SetMinSize(wxSize(MaxWidth + 5, CTRL_HEIGHT));
            }
        }
    }
}

//We can send a message to the parent that this window is destroyed.
bool RunForm::Destroy()
{
    bool bResult = wxWindow::Destroy();

    MyEvent my_event( this );
    my_event.m_bRunFormWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void RunForm::SetDataValue(wxString sIdentifier,wxString sData)
{

    bool bState=false;
    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {
                case CTL_NULL :

                    break;
                case CTL_SPACER :
                    m_CtrlDataItem[index].m_sData = sData;
                    break;
                case CTL_STATIC :
                    m_CtrlDataItem[index].m_sData = sData;
                    break;
                case CTL_TEXT :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].textCtl->SetValue(sData);
                    break;
                case CTL_MULTI_TEXT :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].textCtl->SetValue(sData);
                    break;
                case CTL_SELECTION_LOOKUP_NAME :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_SELECTION_ADDITIVE :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_SELECTION :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_CHECKBOX :
                    m_CtrlDataItem[index].m_sData = sData;
                    sData.Lower();

                    if (sData == "true")
                        bState = true;
                    else if (sData == "false")
                        bState = false;

                    m_CtrlDataItem[index].pCheckBox->SetValue(bState);
                    break;
                case CTL_DATE :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].datePickerCtl->SetValue(Utility::StringToDate(sData));
                    break;
                case CTL_WEBLINK :
                    m_CtrlDataItem[index].m_sData = sData;
                    m_CtrlDataItem[index].linkCtl->SetURL(sData);
                    m_CtrlDataItem[index].linkCtl->SetLabel(sData);
                    break;
                default:
                    break;
            }

            return;

        }
    }
}

wxString RunForm::GetDataValue(wxString sIdentifier)
{

    bool bState=false;
    wxString sTempData="";

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {

                case CTL_SPACER :
                        return m_CtrlDataItem[index].m_sData;

                case CTL_STATIC :
                    return m_CtrlDataItem[index].m_sData;

                case CTL_TEXT :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].textCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_MULTI_TEXT :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].textCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_SELECTION_LOOKUP_NAME :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_SELECTION_ADDITIVE :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_SELECTION :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].m_sData;

                case CTL_CHECKBOX :

                    bState = m_CtrlDataItem[index].pCheckBox->GetValue();
                    sTempData="false";

                    if (bState)
                        sTempData = "true";
                    else if (!bState)
                        sTempData = "false";
                    m_CtrlDataItem[index].m_sData = sTempData;
                    return sTempData;

                case CTL_DATE :
                    m_CtrlDataItem[index].m_sData = Utility::DateToString(m_CtrlDataItem[index].datePickerCtl->GetValue());
                    return m_CtrlDataItem[index].m_sData;

                case CTL_WEBLINK :
                    m_CtrlDataItem[index].m_sData = m_CtrlDataItem[index].linkCtl->GetURL();
                    return m_CtrlDataItem[index].m_sData;

                default:
                    break;
            }
        }
    }
    return "";
}

void* RunForm::GetControl(wxString sIdentifier)
{

    bool bState=false;
    wxString sTempData="";

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {
                case CTL_SPACER :
                    return (wxStaticText*)m_CtrlDataItem[index].TitleCtl;
                case CTL_STATIC :
                    return (wxStaticText*)m_CtrlDataItem[index].TitleCtl;
                case CTL_TEXT :
                    return (wxTextCtrl*)m_CtrlDataItem[index].textCtl;
                case CTL_MULTI_TEXT :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_SELECTION_LOOKUP_NAME :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_SELECTION_ADDITIVE :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_SELECTION :
                    return (wxComboBox*)m_CtrlDataItem[index].comCtl;
                case CTL_CHECKBOX :
                    return (wxCheckBox*)m_CtrlDataItem[index].pCheckBox;
                case CTL_DATE :
                    return (wxDatePickerCtrl*)m_CtrlDataItem[index].datePickerCtl;
                case CTL_WEBLINK :
                    return (wxHyperlinkCtrl*)m_CtrlDataItem[index].linkCtl;
                default:
                    break;
            }
        }
    }

    return nullptr;
}

bool RunForm::GetCheckState(wxString sIdentifier)
{
    bool bState=false;

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {

                case CTL_CHECKBOX :
                    return m_CtrlDataItem[index].pCheckBox->GetValue();;

                default:

                    break;
            }
        }
    }
    return false;
}

void RunForm::SetCheckState(wxString sIdentifier, bool bCheckState){

    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            //Search the type of control
            switch(m_CtrlDataItem[index].iTypeOfControl) {

                case CTL_CHECKBOX :
                    m_CtrlDataItem[index].pCheckBox->SetValue(bCheckState);
                default:

                    break;
            }
        }
    }
}

int RunForm::GetCtlIndex(wxString sIdentifier){
    for (int index=0; index < m_CtrlDataItem.GetCount();index++) {
        if (sIdentifier == m_CtrlDataItem[index].m_sIdentifier)
            return index;
    }
    return wxNOT_FOUND;
}

//Set addition flags for the data: PASSWORD - READONLY
void RunForm::SetFlags(wxString sIdentifier, wxString sFlags)
{
    for (int index=0; index < m_CtrlDataItem.GetCount();index++) {
        if (sIdentifier == m_CtrlDataItem[index].m_sIdentifier){
            m_CtrlDataItem[index].Flags=sFlags;
            return;
        }
    }
}


//ACTIONS:

void RunForm::OnButtonClick( wxCommandEvent& event )
{
    myButton * pButton = (myButton*) event.GetEventObject();
    if(pButton!= nullptr){
        wxString sAction = pButton->GetAction();

        if(sAction=="ACTION_EXIT"){
            Close(true);

            //Send this event to the parent.
            MyEvent my_event( this );
            my_event.m_bDestroyed = true;
            GetParent()->ProcessWindowEvent( my_event );

        }
        else if(sAction=="ACTION_INSERT") {
            if(m_TableList.IsEmpty()){
                wxLogMessage("Unable to extract table from query, check form query.");
            }else if (m_TableList.GetCount()>1){
                wxLogMessage("The form query references more than one table, insert can only work on one table.");
            }
            else if(m_TableList.GetCount()==1){
                //wxLogMessage(sArray[0]);
                InsertItem(m_TableList[0]);
                Close(true);
            }

        }
        else if(sAction=="ACTION_UPDATE") {
            if(m_TableList.IsEmpty()){
                wxLogMessage("Unable to extract table from query, check form query.");
            }else if (m_TableList.GetCount()>1){
                wxLogMessage("The form query references more than one table, insert can only work on one table.");
            }
            else if(m_TableList.GetCount()==1){
                //wxLogMessage(sArray[0]);
                UpdateItem(m_TableList[0], m_sCurrentId);
                Close(true);
            }
        }
        else if(sAction=="ACTION_DELETE") {
            if(m_TableList.IsEmpty()){
                wxLogMessage("Unable to extract table from query, check form query.");
            }else if (m_TableList.GetCount()>1){
                wxLogMessage("The form query references more than one table, insert can only work on one table.");
            }
            else if(m_TableList.GetCount()==1){
                DeleteItem(m_TableList[0], m_sCurrentId);
                Close(true);
            }
        }
        else if(sAction=="ACTION_OPEN_FORM") {
            MyEvent my_event( this );
            my_event.m_sFormId=pButton->GetFormId();
            my_event.m_sBuildQuery=pButton->GetBuildQuery();
            my_event.m_bRunForm=true;
            GetParent()->ProcessWindowEvent( my_event );
        }
        else
            wxLogMessage(sAction);
    }
}

//Before you dropdown, save the combo contents.
void RunForm::OnComboDropDown( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    m_sOldSelectionText = combo->GetValue();

}

void RunForm::OnComboChange( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString value = combo->GetStringSelection();

       if(m_sOldSelectionText.IsEmpty())
          combo->SetValue(value);
       else{
          m_sOldSelectionText << " - " << value;
          combo->SetValue(m_sOldSelectionText);
      }
}

RunForm::~RunForm(){

}

void RunForm::InsertItem(wxString sTableName){


    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    try{
        // Connect to the sample database.
        Connection conn(false);

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {

            ParseSpecialCharacters();

            wxString queryString;
            int count = m_CtrlDataItem.GetCount();

            if (count>0){

                queryString = "INSERT INTO " +sTableName + " (";

                for(int i=0;i<count;i++){

                    //If we don't have a fieldName, then is might be a button, skip item
                    if(m_CtrlDataItem[i].fieldName.IsEmpty())
                        continue;

                    if(Utility::IsReservedMySQLWord(m_CtrlDataItem[i].fieldName)){
                        wxString msg = "The field is a mysql reserved word: "+m_CtrlDataItem[i].fieldName;
                        wxLogMessage(msg);
                        return;
                    }

                    queryString = queryString + m_CtrlDataItem[i].fieldName + ",";
                }

                int LenOfQuery = queryString.Length();
                queryString = queryString.Left(LenOfQuery-1);

                queryString = queryString + ") VALUES (";

                for(int i=0;i<count;i++){

                    //If we don't have a fieldName, then is might be a button, skip item
                    if(m_CtrlDataItem[i].fieldName.IsEmpty())
                        continue;

                    wxString sValue;
                    if(m_CtrlDataItem[i].textCtl != nullptr)
                        sValue = m_CtrlDataItem[i].textCtl->GetValue();
                    else if(m_CtrlDataItem[i].comCtl != nullptr){

                        if(m_CtrlDataItem[i].iTypeOfControl == CTL_SELECTION_LOOKUP_NAME) {

                            //We need to get this working, it's doing a linked name at the moment.
                            sValue = m_CtrlDataItem[i].comCtl->GetValue();

                            wxString tableName="";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Linked_Table",tableName);

                            wxString fieldName = "";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Display_Field",fieldName);

                            wxArrayString sTableResult;
                            Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName,sValue);
                            if(sTableResult.GetCount()>0)
                                sValue = sTableResult[0];
                            else
                                sValue = "0";// If we have an ID that can be NULL, then give it a value of 0

                        }else if(m_CtrlDataItem[i].iTypeOfControl == CTL_SELECTION_LINKED_NAME) {

                            sValue = m_CtrlDataItem[i].comCtl->GetValue();

                            wxString tableName="";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Linked_Table",tableName);

                            wxString fieldName = "";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Display_Field",fieldName);

                            wxArrayString sTableResult;
                            Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName,sValue);
                            if(sTableResult.GetCount()>0)
                                sValue = sTableResult[0];
                            else
                                sValue = "0";// If we have an ID that can be NULL, then give it a value of 0
                        }
                        else
                            sValue = m_CtrlDataItem[i].comCtl->GetValue();
                    }
                    else if(m_CtrlDataItem[i].datePickerCtl != nullptr)
                        sValue = Utility::DateToString(m_CtrlDataItem[i].datePickerCtl->GetValue());
                    else if(m_CtrlDataItem[i].linkCtl != nullptr)
                        sValue = m_CtrlDataItem[i].linkCtl->GetURL();


                    //If we have a linked selection, we don't save the value from the combo text, we save the ID from the table
                    // The value in the combo will be the table name, so it doesn't matter here if we have a SELECTION_LINKED_ID or SELECTION_LINKED_NAME
                    // These are important when we fill our combo with choices.
                    if(m_CtrlDataItem[i].fieldType=="int")
                        queryString = queryString + sValue + ",";
                    else
                        queryString = queryString + "'" + sValue + "',";
                }
            }

            int LenOfQuery = queryString.Length();
            queryString = queryString.Left(LenOfQuery-1);

            queryString += ")";

            Query query = conn.query(queryString);
            if(!query.execute()){
                wxLogMessage("Failed to insert.");
            }
        }

    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
        //f->SetStatusText("Error: "+ wxString(er.what()));

    } catch (const BadConversion& er) {
        // Handle bad conversions
        //f->SetStatusText("Error: "+ wxString(er.what()));

    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    }
}

void RunForm::DeleteItem(wxString sTableName, wxString sId){
    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    try{
        // Connect to the sample database.
        Connection conn(false);


        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {

            wxString queryString = " DELETE FROM " + sTableName + " WHERE " + sTableName + "Id = " + sId;
            Query query = conn.query(queryString);
            query.execute();
        }

    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
    } catch (const BadConversion& er) {
        // Handle bad conversions
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
    }
}

void RunForm::UpdateItem(wxString sTableName, wxString sId){

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    try{
        // Connect to the sample database.
        Connection conn(false);


        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {

            ParseSpecialCharacters();

            wxString queryString = "UPDATE " + sTableName + " SET ";

            // Run through the array and create the fields
            int count = m_CtrlDataItem.GetCount();
            int totalFields=0; // Count the total fields.

            if (count>0){

                for(int i=0;i<count;i++) {

                    //If we don't have a fieldName, then is might be a button, skip item
                    if (m_CtrlDataItem[i].fieldName.IsEmpty())
                        continue;

                    totalFields++;
                }

                for(int i=0;i<count;i++){


                    //If we don't have a fieldName, then is might be a button, skip item
                    if(m_CtrlDataItem[i].fieldName.IsEmpty())
                        continue;

                    wxString sValue;
                    if(m_CtrlDataItem[i].textCtl != nullptr)
                        sValue = m_CtrlDataItem[i].textCtl->GetValue();
                    else if(m_CtrlDataItem[i].comCtl != nullptr){

                        if(m_CtrlDataItem[i].iTypeOfControl == CTL_SELECTION_LOOKUP_NAME) {

                            sValue = m_CtrlDataItem[i].comCtl->GetValue();

                            wxString tableName="";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Linked_Table",tableName);

                            wxString fieldName = "";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Display_Field",fieldName);

                            wxArrayString sTableResult;
                            Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName,sValue);
                            if(sTableResult.GetCount()>0)
                                sValue = sTableResult[0];
                            else
                                sValue = "0";// If we have an ID that can be NULL, then give it a value of 0

                        }else if(m_CtrlDataItem[i].iTypeOfControl == CTL_SELECTION_LINKED_NAME) {

                            sValue = m_CtrlDataItem[i].comCtl->GetValue();

                            wxString tableName="";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Linked_Table",tableName);

                            wxString fieldName = "";
                            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[i].m_sControlId,"Display_Field",fieldName);

                            wxArrayString sTableResult;
                            Utility::GetRecordIDFromTableWhereFieldEquals(Settings.sDatabase, sTableResult, tableName, fieldName,sValue);
                            if(sTableResult.GetCount()>0)
                                sValue = sTableResult[0];
                            else
                                sValue = "0";// If we have an ID that can be NULL, then give it a value of 0
                        }
                        else
                            sValue = m_CtrlDataItem[i].comCtl->GetValue();
                    }

                    else if(m_CtrlDataItem[i].datePickerCtl != nullptr)
                        sValue = Utility::DateToString(m_CtrlDataItem[i].datePickerCtl->GetValue());
                    else if(m_CtrlDataItem[i].linkCtl != nullptr)
                        sValue = m_CtrlDataItem[i].linkCtl->GetURL();


                    //If we have a linked selection, we don't save the value from the combo text, we save the ID from the table
                    // The value in the combo will be the table name, so it doesn't matter here if we have a SELECTION_LINKED_ID or SELECTION_LINKED_NAME
                    // These are important when we fill our combo with choices.
                    if(m_CtrlDataItem[i].fieldType=="int")
                            queryString = queryString + m_CtrlDataItem[i].fieldName + " = " + sValue + " ,";
                    else
                        queryString = queryString + m_CtrlDataItem[i].fieldName + " = '" + sValue + "' ,";
                }
            }
            //Remove last ',';
            int LenOfQuery = queryString.Length();
            queryString = queryString.Left(LenOfQuery-1);

            queryString = queryString + "WHERE " + sTableName + "Id = " + sId;
            Query query = conn.query(queryString);
            query.execute();
        }

    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
    } catch (const BadConversion& er) {
        // Handle bad conversions
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
    }
}


//This only removed " and ' from the field, this is not allowed in an entry.
void RunForm::ParseSpecialCharacters()
{


    // Run through the array and create the fields
    int count = m_CtrlDataItem.GetCount();

    if (count>0){

        for(int i=0;i<count;i++){

            wxString st;

            if(m_CtrlDataItem[i].textCtl != nullptr)
                st = m_CtrlDataItem[i].textCtl->GetValue();
            else if(m_CtrlDataItem[i].comCtl != nullptr)
                st = m_CtrlDataItem[i].comCtl->GetValue();


            st=Utility::Escape(st);

            if(m_CtrlDataItem[i].textCtl != nullptr)
                m_CtrlDataItem[i].textCtl->SetValue(st);
            else if(m_CtrlDataItem[i].comCtl != nullptr)
                m_CtrlDataItem[i].comCtl->SetValue(st);
        }
    }
}

void RunForm::OnMyEvent(MyEvent& event) {

    if (event.m_bButtonClicked) {
        //We clicked the record selector;
        //Load the fields.
        m_sCurrentId = event.m_sTableId;
        LoadFields(m_sCurrentId);

    }
}

//Load the form fields directly from the database rather than passing them in from the form given a passwordId
void RunForm::LoadFields(wxString sId)
{

    if(sId.IsEmpty())
        return;

    wxString database(Settings.sDatabase);
    wxString server(Settings.sServer);
    wxString user(Settings.sDatabaseUser);
    wxString pass(Settings.sPassword);

    // Connect to the sample database.
    Connection conn(false);
    try{

        if (conn.connect((const_cast<char*>((const char*)database.mb_str())),
                         (const_cast<char*>((const char*)server.mb_str())),
                         (const_cast<char*>((const char*)user.mb_str())),
                         (const_cast<char*>((const char*)pass.mb_str())))) {

            if(m_TableList.GetCount()>0){


                Query query = conn.query("select * from " + m_TableList[0] + " WHERE "+m_TableList[0]+"Id = " + sId);

                StoreQueryResult res = query.store();

                // Display results
                if (res) {

                    //There will only be one row
                    // Get each row in result set, and print its contents
                    for (size_t i = 0; i < res.num_rows(); ++i) {

                        //Add a new row to the grid control.
                        int count = m_CtrlDataItem.GetCount();


                        if (count > 0) {

                            for (int index = 0; index < count; index++) {

                                //If we don't have a fieldName, then is might be a button, skip item
                                if(m_CtrlDataItem[index].fieldName.IsEmpty())
                                    continue;

                                wxString fieldName = m_CtrlDataItem[index].fieldName;
                                wxString strData1(res[i][fieldName], wxConvUTF8);

                                wxString defaultValue = m_CtrlDataItem[index].fieldDefault;

                                //Make sure we don't have an empty field because of a bug that cause field left to overwright the right field
                                if (strData1.IsEmpty()) {
                                    //This is where we can place a default value only if there is no value in the database.
                                    if (!defaultValue.IsEmpty())
                                        strData1 = defaultValue;
                                    else if (m_CtrlDataItem[index].iTypeOfControl == CTL_DATE ) { //Check to see if the type is date, then set with current date.
                                        wxDateTime now = wxDateTime::Now();
                                        wxString dt = now.FormatDate();
                                        strData1 = dt;
                                    } else if (m_CtrlDataItem[index].iTypeOfControl == CTL_TIME ) { //Check to see if the type is date, then set with current date.
                                        wxDateTime now = wxDateTime::Now();
                                        wxString dt = now.FormatTime();
                                        strData1 = dt;
                                    } else if (m_CtrlDataItem[index].fieldType ==
                                               "DATETIME") { //Check to see if the type is date, then set with current date.

                                    } else if (m_CtrlDataItem[index].fieldType ==
                                               "TIMESTAMP") { //Check to see if the type is date, then set with current date.

                                    } else if (m_CtrlDataItem[index].fieldType ==
                                               "YEAR") { //Check to see if the type is date, then set with current date.

                                    }
                                }

                                if (m_CtrlDataItem[index].textCtl != nullptr)
                                    m_CtrlDataItem[index].textCtl->SetValue(strData1);
                                else if (m_CtrlDataItem[index].comCtl != nullptr) {

                                    //If we have a linked ID, we want to show the table name, not the ID
                                    if (m_CtrlDataItem[index].iTypeOfControl == CTL_SELECTION_LINKED_NAME) {

                                        wxString tableName="";
                                        Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Linked_Table",tableName);

                                        wxString fieldName = "";
                                        Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Display_Field",fieldName);

                                        wxArrayString sTableResult;
                                        strData1.Trim();

                                        if (Utility::GetFieldFromTableWhereFieldEquals(Settings.sDatabase, sTableResult,
                                                                                       tableName, fieldName,
                                                                                       tableName + "Id", strData1))
                                            m_CtrlDataItem[index].comCtl->SetValue(sTableResult[0]);

                                    } else
                                        m_CtrlDataItem[index].comCtl->SetValue(strData1);
                                } else if (m_CtrlDataItem[index].datePickerCtl != nullptr)
                                    m_CtrlDataItem[index].datePickerCtl->SetValue(Utility::StringToDate(strData1));
                                else if (m_CtrlDataItem[index].linkCtl != nullptr) {
                                    m_CtrlDataItem[index].linkCtl->SetURL(strData1);
                                    m_CtrlDataItem[index].linkCtl->SetLabel(strData1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }catch (BadQuery& er) { // handle any connection or
        // query errors that may come up
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const BadConversion& er) {
        // Handle bad conversions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    } catch (const Exception& er) {
        // Catch-all for any other MySQL++ exceptions
        //f->SetStatusText("Error: "+ wxString(er.what()));
    }
}

