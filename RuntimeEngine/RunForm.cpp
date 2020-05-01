//
// Created by Nicholas Zounis on 16/3/20.
//
#include <wx/wxprec.h>
#include<wx/wx.h>
#include <wx/hyperlink.h>
#include <wx/datectrl.h>

#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "../Shared/global.h"

#include "RunForm.h"


wxBEGIN_EVENT_TABLE(RunForm, wxFrame)

wxEND_EVENT_TABLE()


RunForm::RunForm( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_CalculatedHeightWindow=0;
    m_sOldSelectionText="";
    wxString m_sFormId=""; //Set for testing

    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );
}
void RunForm::Create()
{
    LoadControlObjects();
    RenderAllControls();
}

wxString RunForm::GetFormID()
{
    return m_sFormId; //This will return the current form ID from the selection control
}

void RunForm::SetFormID(wxString Id)
{
    m_sFormId = Id;
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
}

wxString RunForm::GetQuery()
{
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
                pCtl->sIdentifier = sControlId;

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
                Utility::LoadTableData(Settings.sDatabase,"usr_controls",sControlId,wxT("Field"),sField);
                pCtl->m_sField=sField;
/*
                int lxPos = Utility::StringToInt(xPos);
                int lyPos = Utility::StringToInt(yPos);

                if(lxPos> m_sizeWinObjectsExtend.x){
                    m_sizeWinObjectsExtend.x = lxPos;
                }
                if(lyPos> m_sizeWinObjectsExtend.y){
                    m_sizeWinObjectsExtend.y = lyPos;
                }

                wxPoint pt(Utility::StringToInt(xPos),Utility::StringToInt(yPos));
                pCtl->SetControlPosition(pt);*/

                m_CtrlDataItem.Add(pCtl);
            }
        }
        //RedrawControlObjects(); // Draw them to the screen.
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

void RunForm::OnComboCloseUp( wxCommandEvent& event )
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();


    //Remove the comments from the combo.
    wxString value = combo->GetStringSelection();
    int i = value.find('[',0);

    if(i!= wxNOT_FOUND){
        value = value.Left(i);
        value = value.Trim(true);
        combo->SetValue(value);
    }
}

RunForm::~RunForm(){

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
    //Clear found flags, just in case
    for(int idx=0; idx < m_CtrlDataItem.GetCount(); idx++)
        m_CtrlDataItem[idx].bFound=false;

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
    bool bWebLink = false;
    unsigned long style=0;


    sFlags =  m_CtrlDataItem[index].Flags; // Get additional flags for the control
    switch(m_CtrlDataItem[index].iTypeOfControl) {
        case CTL_NULL :

            break;
        case CTL_SPACER :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].SpacerCtl = new wxStaticText( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 );
            //m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            //m_CtrlDataItem[index].TitleCtl->SetSize(500,30);
            //m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].SpacerCtl, 1, wxALL, BORDER_WIDTH);

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            break;

        case CTL_STATIC :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );

            m_CtrlDataItem[index].SpacerCtl = new wxStaticText( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 );
            gSizer1->Add( m_CtrlDataItem[index].SpacerCtl, 0, wxALL, BORDER_WIDTH);

            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            //m_CtrlDataItem[index].TitleCtl->SetSize(500,30);
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 1, wxALL, BORDER_WIDTH);

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
            break;
        case CTL_TEXT :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            if (Utility::HasFlag(m_CtrlDataItem[index].Flags,"READONLY"))
                style = wxTE_READONLY;
                //The only place we actually want to see the password is when we view the table.
            else if(Utility::HasFlag(m_CtrlDataItem[index].Flags,"PASSWORD") )
                style = wxTE_PASSWORD;

            m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
            m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
            gSizer1->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;


            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_MULTI_TEXT :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
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

            m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
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
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            m_asSelectionItemArray.Clear();

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Linked_Table",sTableName);
            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Display_Field",sFieldName);

            sFlags = "SELECTION_LOOKUP_NAME{"+sTableName+";"+sFieldName+";}";
            Utility::ExtractSelectionLookupItemsName(m_asSelectionItemArray,sFlags);

            m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style );
            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);

            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_SELECTION_LOOKUP_NAME :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            m_asSelectionItemArray.Clear();

            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Linked_Table",sTableName);
            Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_CtrlDataItem[index].m_sControlId,"Display_Field",sFieldName);

            sFlags = "SELECTION_LOOKUP_NAME{"+sTableName+";"+sFieldName+";}";
            Utility::ExtractSelectionLookupItemsName(m_asSelectionItemArray,sFlags);

            m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style );
            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);

            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);
                m_CtrlDataItem[index].comCtl->Append(m_asSelectionItemArray[index]);

            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION_ADDITIVE :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            style=m_CtrlDataItem[index].comCtl->GetWindowStyle();
            //Create a wxComboCtrl. We need to fill it with the values extracted from

            //Extract all the selection items.
            m_asSelectionItemArray.Clear();
            Utility::ExtractSelectionItems(m_asSelectionItemArray,sFlags);

            // If there is no selection items in the flags, they will be in usr_control_items table.
            if(m_asSelectionItemArray.IsEmpty())
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

            m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);


            gSizer1->Add( m_CtrlDataItem[index].comCtl, ALLOW_TO_GROW , wxEXPAND, BORDER_WIDTH);


            //Fill the list box with the selection items.
            Utility::LoadComboFromStringArray(m_CtrlDataItem[index].comCtl,m_asSelectionItemArray);
            //for ( int iIdx=0; iIdx<sSelectionItemArray.GetCount(); iIdx++ )
            //m_CtrlDataItem[index].comCtl->Append(sSelectionItemArray[iIdx]);

            //Make sure you set the default value after you append selection items.
            if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                m_CtrlDataItem[index].comCtl->SetValue(m_CtrlDataItem[index].fieldDefault);


            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            //Attache handlers to buttons

            // Connect a closeup event so I can remove the comments.
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( RunForm::OnComboCloseUp ), nullptr, this );

            //These events will prevent the replacement of text and add them together.

            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( RunForm::OnComboChange ), nullptr, this );
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( RunForm::OnComboDropDown ), nullptr, this );


            m_MainFormSizer->Add( gSizer1, 0, wxEXPAND, BORDER_WIDTH );
            break;

        case CTL_SELECTION :

            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);

            style=0;
            //Create a wxComboCtrl. We need to fill it with the values extracted from

            //Extract all the selection items.
            m_asSelectionItemArray.Clear();
            Utility::ExtractSelectionItems(m_asSelectionItemArray,sFlags);

            // If there is no selection items in the flags, they will be in usr_control_items table.
            if(m_asSelectionItemArray.IsEmpty())
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

            m_CtrlDataItem[index].comCtl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,style);


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
            m_CtrlDataItem[index].comCtl->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( RunForm::OnComboCloseUp ), nullptr, this );



            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_CHECKBOX :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            m_CtrlDataItem[index].pCheckBox = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
            gSizer1->Add( m_CtrlDataItem[index].pCheckBox, 0, wxALL, BORDER_WIDTH );

            m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;

            m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );
            break;
        case CTL_DATE :
            gSizer1 = new wxBoxSizer( wxHORIZONTAL );
            m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
            m_CtrlDataItem[index].TitleCtl->Wrap( -1 );
            m_CtrlDataItem[index].TitleCtl->SetLabel(m_CtrlDataItem[index].m_sLabel);
            gSizer1->Add( m_CtrlDataItem[index].TitleCtl, 0, wxALL, BORDER_WIDTH);
            style=0;

            style = wxDP_DEFAULT | wxDP_DROPDOWN | wxDP_SHOWCENTURY;
            m_CtrlDataItem[index].datePickerCtl = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, style );

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
                m_CtrlDataItem[index].TitleCtl = new wxStaticText( this, wxID_ANY, m_CtrlDataItem[index].Title, wxDefaultPosition, wxDefaultSize, 0 );
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
                    m_CtrlDataItem[index].linkCtl = new wxHyperlinkCtrl( this, wxID_ANY,"weblink","https://www.google.com", wxDefaultPosition, wxDefaultSize, wxHL_CONTEXTMENU | wxHL_DEFAULT_STYLE ,wxHyperlinkCtrlNameStr);
                    m_CtrlDataItem[index].linkCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                    gSizer1->Add( m_CtrlDataItem[index].linkCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );
                    m_CtrlDataItem[index].linkCtl->SetURL("");
                    m_CtrlDataItem[index].linkCtl->SetLabel("");

                    if(!m_CtrlDataItem[index].fieldDefault.IsEmpty()){
                        m_CtrlDataItem[index].linkCtl->SetURL(m_CtrlDataItem[index].fieldDefault);
                        m_CtrlDataItem[index].linkCtl->SetLabel(m_CtrlDataItem[index].fieldDefault);
                    }
                }else{

                    m_CtrlDataItem[index].textCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, style );
                    m_CtrlDataItem[index].textCtl->SetMinSize( wxSize( TEXTCTL_WIDTH,CTRL_HEIGHT ) );
                    gSizer1->Add( m_CtrlDataItem[index].textCtl,ALLOW_TO_GROW, wxEXPAND, BORDER_WIDTH );


                    if(!m_CtrlDataItem[index].fieldDefault.IsEmpty())
                        m_CtrlDataItem[index].textCtl->SetValue(m_CtrlDataItem[index].fieldDefault);
                }

                m_CalculatedHeightWindow += CTRL_HEIGHT + BORDER_WIDTH + BORDER_WIDTH;
                m_MainFormSizer->Add( gSizer1,0, wxEXPAND, BORDER_WIDTH );

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
    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bRunFormWasDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void RunForm::SetDataValue(wxString sIdentifier,wxString sData)
{

    bool bState=false;
    for (int index=0; index < m_CtrlDataItem.GetCount();index++){
        if(sIdentifier == m_CtrlDataItem[index].sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {
                case CTL_NULL :

                    break;
                case CTL_SPACER :
                    m_CtrlDataItem[index].sData = sData;
                    break;
                case CTL_STATIC :
                    m_CtrlDataItem[index].sData = sData;
                    break;
                case CTL_TEXT :
                    m_CtrlDataItem[index].sData = sData;
                    m_CtrlDataItem[index].textCtl->SetValue(sData);
                    break;
                case CTL_MULTI_TEXT :
                    m_CtrlDataItem[index].sData = sData;
                    m_CtrlDataItem[index].textCtl->SetValue(sData);
                    break;
                case CTL_SELECTION_LOOKUP_NAME :
                    m_CtrlDataItem[index].sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_SELECTION_ADDITIVE :
                    m_CtrlDataItem[index].sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_SELECTION :
                    m_CtrlDataItem[index].sData = sData;
                    m_CtrlDataItem[index].comCtl->SetValue(sData);
                    break;
                case CTL_CHECKBOX :
                    m_CtrlDataItem[index].sData = sData;
                    sData.Lower();

                    if (sData == "true")
                        bState = true;
                    else if (sData == "false")
                        bState = false;

                    m_CtrlDataItem[index].pCheckBox->SetValue(bState);
                    break;
                case CTL_DATE :
                    m_CtrlDataItem[index].sData = sData;
                    m_CtrlDataItem[index].datePickerCtl->SetValue(Utility::StringToDate(sData));
                    break;
                case CTL_WEBLINK :
                    m_CtrlDataItem[index].sData = sData;
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
        if(sIdentifier == m_CtrlDataItem[index].sIdentifier){
            //Search the type of control

            switch(m_CtrlDataItem[index].iTypeOfControl) {

                case CTL_SPACER :
                        return m_CtrlDataItem[index].sData;

                case CTL_STATIC :
                    return m_CtrlDataItem[index].sData;

                case CTL_TEXT :
                    m_CtrlDataItem[index].sData = m_CtrlDataItem[index].textCtl->GetValue();
                    return m_CtrlDataItem[index].sData;

                case CTL_MULTI_TEXT :
                    m_CtrlDataItem[index].sData = m_CtrlDataItem[index].textCtl->GetValue();
                    return m_CtrlDataItem[index].sData;

                case CTL_SELECTION_LOOKUP_NAME :
                    m_CtrlDataItem[index].sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].sData;

                case CTL_SELECTION_ADDITIVE :
                    m_CtrlDataItem[index].sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].sData;

                case CTL_SELECTION :
                    m_CtrlDataItem[index].sData = m_CtrlDataItem[index].comCtl->GetValue();
                    return m_CtrlDataItem[index].sData;

                case CTL_CHECKBOX :

                    bState = m_CtrlDataItem[index].pCheckBox->GetValue();
                    sTempData="false";

                    if (bState)
                        sTempData = "true";
                    else if (!bState)
                        sTempData = "false";
                    m_CtrlDataItem[index].sData = sTempData;
                    return sTempData;

                case CTL_DATE :
                    m_CtrlDataItem[index].sData = Utility::DateToString(m_CtrlDataItem[index].datePickerCtl->GetValue());
                    return m_CtrlDataItem[index].sData;

                case CTL_WEBLINK :
                    m_CtrlDataItem[index].sData = m_CtrlDataItem[index].linkCtl->GetURL();
                    return m_CtrlDataItem[index].sData;

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
        if(sIdentifier == m_CtrlDataItem[index].sIdentifier){
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
        if(sIdentifier == m_CtrlDataItem[index].sIdentifier){
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
        if(sIdentifier == m_CtrlDataItem[index].sIdentifier){
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
        if (sIdentifier == m_CtrlDataItem[index].sIdentifier)
            return index;
    }
    return wxNOT_FOUND;
}

//Set addition flags for the data: PASSWORD - READONLY
void RunForm::SetFlags(wxString sIdentifier, wxString sFlags)
{
    for (int index=0; index < m_CtrlDataItem.GetCount();index++) {
        if (sIdentifier == m_CtrlDataItem[index].sIdentifier){
            m_CtrlDataItem[index].Flags=sFlags;
            return;
        }
    }
}
