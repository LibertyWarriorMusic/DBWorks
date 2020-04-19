

// THIS MUST COME FIRST BEFORE YOU INCLUDE MAIN.H
#include <wx/wxprec.h>
#include<wx/wx.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/combo.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <wx/frame.h>
#include <wx/stdpaths.h>
#include <wx/toolbar.h>
#include <wx/dynarray.h>


#include "../global.h"
#include "../HtmlHelp.h"


//Add my own classes here
#include "GenericQueryGrid.h"
#include "GenericItemForm.h"
#include "../MyEvent.h"
#include "../Utility.h"

#include "DBGrid.h"
#include "GenericTable.h"


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayTableField);


enum {
    ID_TOOL_ADD= wxID_HIGHEST + 10000,
    ID_TOOL_EDIT,
    ID_TOOL_DELETE,
    ID_TOOL_VIEW,
    ID_HELP

};

wxBEGIN_EVENT_TABLE(GenericTable, wxFrame)
    EVT_MYEVENT(GenericTable::OnMyEvent)
    EVT_TOOL(ID_TOOL_ADD, GenericTable::OnbAddItem)
    EVT_TOOL(ID_TOOL_EDIT, GenericTable::OnbEditItem)
    EVT_TOOL(ID_TOOL_DELETE, GenericTable::OnbDeleteItem)
    EVT_TOOL(ID_TOOL_VIEW, GenericTable::OnbViewItem)
    EVT_TOOL(ID_HELP, GenericTable::OnbHelp)
wxEND_EVENT_TABLE()

//=============
// MyFrame Class Methods
//

GenericTable::GenericTable( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_StatusBar = nullptr;
    m_Close = nullptr;
    m_Grid = nullptr;
    m_DrawPane = nullptr;
    m_Toolbar = nullptr;
    m_HtmlWin = nullptr;
    m_txtFilter=nullptr;
    m_ComboFilter=nullptr;
    m_pQueryGrid=nullptr;
    m_pFormItem= nullptr;
    m_sCurrentStoredWhereCondition="";
    m_iTempRowIndex=-1;
    m_sTempFormQuery="";
}


void GenericTable::AddField(const wxString& title, const wxString& field, const wxString& type, const wxString& flag, const wxString& defaultVal, const wxString& KeyVal, const wxString& ExtraVal, const wxString &nullVal)
{
    
    auto * tableField = new TableField();

    tableField->fieldName=field;
    tableField->fieldType = type;
    tableField->fieldExtra = ExtraVal;
    tableField->fieldKey = KeyVal;
    tableField->fieldNull=nullVal;
    tableField->fieldDefault = defaultVal;
    tableField->Title=title;
    tableField->Flags = flag;

    m_FieldArray.Add(tableField);
    
}
bool GenericTable::Create()
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    //-------------------------------------------------
    // The main form sizers, panels and controls
    //This is the sizer that is directly attached to the main form

        m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

            //Create the spread sheet grid
            m_Grid = new DBGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, (unsigned)wxVSCROLL | (unsigned)wxFULL_REPAINT_ON_RESIZE);

            int count = m_FieldArray.GetCount();

            if (count>0){

                wxString msg;
                msg << count;

                //wxMessageBox(msg);

                for(int index=0;index<count;index++){
                    //wxMessageBox(m_FieldArray[index].title + "-" + m_FieldArray[index].fieldName);
                    if(Settings.bShowGridColumnFields && m_sTableName!=SYS_FIELDS)
                         m_Grid->AddItem(m_FieldArray[index].fieldName,m_FieldArray[index].fieldName,m_FieldArray[index].Flags,m_FieldArray[index].fieldDefault, m_FieldArray[index].fieldType, m_FieldArray[index].fieldNull,m_FieldArray[index].fieldKey,m_FieldArray[index].fieldExtra );
                    else
                        m_Grid->AddItem(m_FieldArray[index].Title,m_FieldArray[index].fieldName,m_FieldArray[index].Flags,m_FieldArray[index].fieldDefault, m_FieldArray[index].fieldType, m_FieldArray[index].fieldNull,m_FieldArray[index].fieldKey,m_FieldArray[index].fieldExtra);
                }

            }

            m_Grid->SetEventType(myEVT_MYEVENT);
            m_Grid->CreateFields();
            //Set the column Labels

            //Remove the horizonal scroll bar
            m_Grid->SetWindowStyleFlag( (unsigned)m_Grid->GetWindowStyle() &~ (unsigned)wxHSCROLL );

    //Add the spread sheet directly to the main form box grid..
    m_MainFormSizer->Add( m_Grid, 0, wxGROW, 0);


            //The sizer that holds all the buttons will be placed in the formSizer
            auto* buttonSizer = new wxBoxSizer( wxHORIZONTAL );

            m_Close= new wxButton( this, wxID_ANY, wxT("Close Window"), wxDefaultPosition, wxDefaultSize, 0 );
            buttonSizer->Add( m_Close, 0, wxALL, 5 );


    //Add the button sizer to the main form box grid
    m_MainFormSizer->Add( buttonSizer, 0, wxALL, 0 );

    //Set the sizer to be attached to the main form
    this->SetSizer( m_MainFormSizer );

    //Layout the grid.
    this->Layout();

    //this->Centre( wxBOTH );
    m_StatusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );


    //Attache handlers to buttons
    m_Close->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GenericTable::OnbExitApp ), nullptr, this );

    //Set the grid settings
    m_Grid->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName, m_sTableName+"Id",m_sWhereCondition);


    //If we have an error and the grid faled to load, just destroy it.
    if(!m_Grid->LoadGridFromDatabase()){
        m_Grid->Destroy();
        m_Grid= nullptr;
        return false;
    }

    wxInitAllImageHandlers(); //You need to call this or the images will not load.
    // Get the path to the images

    if(m_Toolbar == nullptr)
        m_Toolbar = CreateToolBar();


    wxBitmap BitMap;

    Utility::LoadBitmap(BitMap,"help.png");
    m_Toolbar->AddTool(ID_HELP, wxT("Help"), BitMap, wxT("Help."));


    if (Utility::IsSystemDatabaseDeveloper() || Utility::IsSystemDatabaseAdministrator() || Utility::IsAdvancedUser()) {
        //======
        //Load images for the toolbar

        Utility::LoadBitmap(BitMap,"add.png");
        m_Toolbar->AddTool(ID_TOOL_ADD, wxT("Add Entry"), BitMap, wxT("Add a new entry"));

        Utility::LoadBitmap(BitMap,"edit.png");
        m_Toolbar->AddTool(ID_TOOL_EDIT, wxT("Edit Selected Entry"), BitMap, wxT("Edit selected entry"));

        Utility::LoadBitmap(BitMap,"delete.png");
        m_Toolbar->AddTool(ID_TOOL_DELETE, wxT("Delete Selected Rows"), BitMap, wxT("Delete seleted entries"));

        Utility::LoadBitmap(BitMap,"view.png");
        m_Toolbar->AddTool(ID_TOOL_VIEW, wxT("View Selected Rows"), BitMap, wxT("View seleted entries"));

    }else if (Utility::IsStandardUser() ){


        Utility::LoadBitmap(BitMap,"add.png");
        m_Toolbar->AddTool(ID_TOOL_ADD, wxT("Add Entry"), BitMap, wxT("Add a new entry"));

        Utility::LoadBitmap(BitMap,"edit.png");
        m_Toolbar->AddTool(ID_TOOL_EDIT, wxT("Edit Selected Entry"), BitMap, wxT("Edit selected entry"));

        Utility::LoadBitmap(BitMap,"view.png");
        m_Toolbar->AddTool(ID_TOOL_VIEW, wxT("View Selected Rows"), BitMap, wxT("View seleted entries"));

        m_Grid->HideColumn(0);

    }else if(Utility::IsGuest()){

        Utility::LoadBitmap(BitMap,"view.png");
        m_Toolbar->AddTool(ID_TOOL_VIEW, wxT("View Selected Rows"), BitMap, wxT("View seleted entries"));

        m_Grid->HideColumn(0);

    }


    if(m_sTableName!="sys_fields"){ // Don't show filter on the field definitions frame
        ///-------------
        //ADD the query combo to the toolbar
        m_txtFilter = new wxStaticText( m_Toolbar, wxID_ANY, Settings.sUsergroup, wxDefaultPosition, wxDefaultSize, 0 );
        m_txtFilter->SetLabel("Filters ");
        m_ComboFilter = new wxComboBox( m_Toolbar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,wxCB_READONLY);
        m_ComboFilter->SetSize(260,-1);
        m_ComboFilter->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( GenericTable::OnUserGroupComboChange ), nullptr, this );

        //This is the default search.
        m_ComboFilter->Append("Show All");
        m_ComboFilter->Append("Menu Filter");

        wxString sTableId = Utility::GetTableIdFromSYS_TABLESByTableName(Settings.sDatabase,m_sTableName);
        Utility::LoadComboUsrFilters(Settings.sDatabase, *m_ComboFilter, sTableId);
        m_ComboFilter->SetStringSelection("Show All");

        m_Toolbar->AddControl(m_txtFilter);
        m_Toolbar->AddControl(m_ComboFilter);

    }


    m_Toolbar->Realize();
    SetToolBar(m_Toolbar);
    SetSize((int)Settings.lMainWindowWidth,(int)Settings.lMainWindowHeight);

    this->SetBackgroundColour(wxColour(77,120,77));
    //This is here for debugging.
    //SetStatusText("Message = " + Settings.Message);

    if (Utility::IsSystemDatabaseDeveloper())
        SetIDTitleName(m_sTableName+"Id *");
    else
        SetIDTitleName("ID");


    return true;
    
}

void GenericTable::OnUserGroupComboChange(wxCommandEvent& event)
{
    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString selection = combo->GetStringSelection();
    ComboDataItem *comboItem = nullptr;

    int index =  combo->GetSelection();
    if(index > 1) //NOTE: The first two entries in the combo don't have client object, but default they are SHOW ALL and FILTER MENU,
        comboItem = (ComboDataItem*)combo->GetClientObject(index);

    if(selection=="Show All"){
        MyEvent eEvent;
        eEvent.m_bRefreshDatabase=true;
        eEvent.m_bShowAll=true;

        //We allready have
        OnMyEvent(eEvent);
    }
    else if(comboItem!= nullptr){

        MyEvent eEvent;
        eEvent.m_bRefreshDatabase=true;
        eEvent.m_sQueryToApply = comboItem->queryDefinition;

        //We allready have
        OnMyEvent(eEvent);
    }
}

void GenericTable::HideIDColumn()
{
    m_Grid->HideCol(0);
}
void GenericTable::SetIDTitleName(wxString sTitle)
{
    m_Grid->SetColLabelValue(0,sTitle);
}
void GenericTable::OnbExitApp(wxCommandEvent& event)
{
    Close(true);
}

GenericTable::~GenericTable()
{

    // Disconnect button handlers
    if (m_Close != nullptr)
        m_Close->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GenericTable::OnbExitApp ), nullptr, this );

    m_FieldArray.Empty();
    
}

//Button Handle functions
void GenericTable::OnbAddItem( wxCommandEvent& event )
{
    //wxArrayInt rowsSelected = m_Grid->GetSelectedRows();
    //int size = rowsSelected.size();

   // if(size==1){
       // int row = rowsSelected[0];
        AddItem(-1);
   // }

}
void GenericTable::AddItem(long rowID)
{
    if(m_pFormItem != nullptr){
        m_pFormItem->Destroy();
        m_pFormItem = nullptr;
    }
    m_pFormItem = new GenericItemForm((wxFrame*) this, -1,"Add Item",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP| (unsigned)wxRESIZE_BORDER);


    int count = m_FieldArray.GetCount();

    if (count>0){

        for(int index=0;index<count;index++)
            m_pFormItem->AddItem(m_FieldArray[index].Title,m_FieldArray[index].fieldName,m_FieldArray[index].Flags,m_FieldArray[index].fieldType,m_FieldArray[index].fieldDefault,m_FieldArray[index].fieldKey,m_FieldArray[index].fieldExtra,m_FieldArray[index].fieldNull);
    }

    //Note: this has to come before CreateField because m_sTableName is referenced in CreateFields() function
    m_pFormItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName,m_sTableName+"Id");

    m_pFormItem->SetUse("ADD");
    m_pFormItem->CreateFields();



    m_pFormItem->Show(true);
    SetStatusText("Add Item.");

}

void GenericTable::OnbEditItem( wxCommandEvent& event )
{
    wxArrayInt rowsSelected = m_Grid->GetSelectedRows();
    int size = rowsSelected.size();
    
    if(size==1){
        int row = rowsSelected[0];
        EditItem(row);
    }
}
void GenericTable::EditItem(long rowID)
{
    if(m_pFormItem != nullptr){
        m_pFormItem->Destroy();
        m_pFormItem = nullptr;
    }

    m_iTempRowIndex = rowID; // Save the row so we now what to refresh.

    m_pFormItem = new GenericItemForm((wxFrame*) this, -1,"Edit Item",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP | (unsigned)wxRESIZE_BORDER);
    int count = m_FieldArray.GetCount();

    if (count>0){

        for(int index=0;index<count;index++)
            m_pFormItem->AddItem(m_FieldArray[index].Title,m_FieldArray[index].fieldName,m_FieldArray[index].Flags,m_FieldArray[index].fieldType,m_FieldArray[index].fieldDefault,m_FieldArray[index].fieldKey,m_FieldArray[index].fieldExtra,m_FieldArray[index].fieldNull );
    }


    m_pFormItem->SetUse("UPDATE");

    m_pFormItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName,m_sTableName+"Id");
    m_pFormItem->CreateFields();


    wxString spasswordId = m_Grid->GetCellValue(rowID,0);
    SetStatusText(spasswordId);

    m_pFormItem->SetID(spasswordId);
    m_pFormItem->Show(true);
    m_pFormItem->LoadFields();
    SetStatusText("Edit Item");

}

void GenericTable::OnbViewItem( wxCommandEvent& event )
{
    wxArrayInt rowsSelected = m_Grid->GetSelectedRows();
    int size = rowsSelected.size();
    if (size==1){
        int row = rowsSelected[0];
        if(size==1)
            ViewItem(row);
    }
}

void GenericTable::ViewItem(long rowID)
{

    // wxPoint(100,100),
    // wxSize(500,410),
    if(m_pFormItem != nullptr){
        m_pFormItem->Destroy();
        m_pFormItem = nullptr;
    }

    m_pFormItem = new GenericItemForm((wxFrame*) this, -1,"View Item",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP | (unsigned)wxRESIZE_BORDER);

    if(m_pFormItem != nullptr){
        //Define the database
        int count = m_FieldArray.GetCount();

        if (count>0){

            for(int index=0;index<count;index++)
                m_pFormItem->AddItem(m_FieldArray[index].Title,m_FieldArray[index].fieldName,m_FieldArray[index].Flags,m_FieldArray[index].fieldType,m_FieldArray[index].fieldDefault,m_FieldArray[index].fieldKey,m_FieldArray[index].fieldExtra,m_FieldArray[index].fieldNull);
        }

        m_pFormItem->SetUse("VIEW");
        m_pFormItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName,m_sTableName+"Id");
        m_pFormItem->CreateFields();




        wxString spasswordId = m_Grid->GetCellValue((int)rowID,0);
        SetStatusText(spasswordId);

        m_pFormItem->SetID(spasswordId);
        m_pFormItem->Show(true);
        m_pFormItem->LoadFields();
        SetStatusText("View Item");
    }
}
//Runs through all the selectd rows on the grid and deletes the entries from the database.
void GenericTable::OnbDeleteItem( wxCommandEvent& event )
{

    auto *dlg = new wxMessageDialog(nullptr, wxT("Are you sure you want to delete the selected record?"), wxT("Delete Recored"),  wxICON_EXCLAMATION|wxYES_NO);

    if ( dlg->ShowModal() == wxID_YES )
        m_Grid->DeleteSelectedRow();

    dlg->Destroy();
    m_Grid->ResizeSpreadSheet();
    this->Layout(); // This is the key, you need to relayout the form.

    MyEvent my_event( this );
    GetParent()->ProcessWindowEvent( my_event );

}

void GenericTable::SetTableDefinition(const wxString tableName, const wxString title, const wxString comments, const wxString whereCondition)
{
    m_sTableName = tableName;
    m_sTitle=title;
    m_sComments=comments;
    m_sWhereCondition=whereCondition;

}



// If we have a store row, we don't need reload all the grids, only updata the signle row.
void GenericTable::Refresh()
{

    //If we are just updating a row, then we don't need to reload the entire grid, just update the single row.
    if(m_iTempRowIndex>-1)
        m_Grid->LoadGridRowFromDatabase(m_iTempRowIndex);
    else
        m_Grid->LoadGridFromDatabase(false,m_sTempQuery);

    this->Layout(); // This is the key, you need to re-layout the form.

    //After the row has been updated, we need to put this back to -1 so the entire grid can be refreshed.
    m_iTempRowIndex = -1;
    m_sTempQuery="";
}

wxString GenericTable::GetCurrentStoredWhereCondition()
{
    return m_sCurrentStoredWhereCondition;
}

void GenericTable::SetCurrentStoredWhereCondition(wxString sWhereCondition)
{
    m_sCurrentStoredWhereCondition = sWhereCondition;
}

void GenericTable::OnParseDocument(wxString sDocument)
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    //This will simply show the passed document
    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(Utility::PassHTMLDocument(sDocument)); //
    m_HtmlWin->Show(true);
}
//We need to add the 'WHERE' but there is nothing to blend for now.
void GenericTable::SetGridWhereCondition(wxString whereToBlend)
{
    wxString userWhere = ""; // There is no userwhere at the moment.
    wxString sWhereCon = "";

    if (whereToBlend.IsEmpty() && !userWhere.IsEmpty()){
        sWhereCon << " where " << userWhere;
        m_Grid->SetGridWhereCondition(sWhereCon); // Make sure you show all records.
    }else if(!whereToBlend.IsEmpty() && userWhere.IsEmpty()){
        sWhereCon << " where " << whereToBlend;
        m_Grid->SetGridWhereCondition(sWhereCon); // Make sure you show all records.
    }
    else if(!whereToBlend.IsEmpty() && !userWhere.IsEmpty()){
        sWhereCon << " where " << whereToBlend << " and " << userWhere;
        m_Grid->SetGridWhereCondition(sWhereCon); // Make sure you show all records.
    }
    else
        m_Grid->SetGridWhereCondition(""); // Make sure we clear it or once we select a filter, it will remain that way even after you show all.
}

void GenericTable::OnbHelp( wxCommandEvent& event )
{
    //NOTE: This is very useful, if you have a help window already up, you can destory it first. However if the window was already destroyed internally (pressing close icon), then this pointer will
    // be pointing to garbage memory and the program will crash if you try and call Destroy().
    if(m_HtmlWin != nullptr)
        m_HtmlWin->Destroy();

    m_HtmlWin = new HtmlHelp((wxFrame*) this, -1, "Help", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
    m_HtmlWin->SetPage(3); //
    m_HtmlWin->Show(true);

}
void GenericTable::RunQuery(wxString sTitle, wxString sFormGridQuery)
{
    m_pQueryGrid = new GenericQueryGrid((wxFrame*) this, -1,sTitle,wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE);

    if (m_pQueryGrid != nullptr){
        m_pQueryGrid->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword);
        m_pQueryGrid->SetFormQuery(sFormGridQuery);
        if(m_pQueryGrid->Create())
            m_pQueryGrid->Show(true);
        else{
            m_pQueryGrid->Close();
            m_pQueryGrid= nullptr;
        }
    }
}
//
DBGrid* GenericTable::GetGrid()
{
    return m_Grid;
}

//We can send a message to the parent that this window is destroyed.
bool GenericTable::Destroy()
{
    if(m_pQueryGrid!= nullptr){
        m_pQueryGrid->Destroy();
        m_pQueryGrid= nullptr;
    }

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    bool bResult = wxFrame::Destroy();
    return bResult;
}

//We created an event to refresh the grid so we can call it from any frame class.
void GenericTable::OnMyEvent(MyEvent& event )
{
    // wxString msg;
    // msg << event.GetEventType();
    // wxMessageBox( msg);
    //NOTE: We get here if resizing or right menu clicking, so we need to create a flag to indicated which.
    if(event.m_bTableFieldDefinitions)
        wxMessageBox("We are in Table from properties.");
    else if(event.m_bOpen)
        ViewItem(event.m_iRow);
    else if (event.m_bEdit)
        EditItem(event.m_iRow);
    else if(event.m_bDestroyed){
        m_pFormItem= nullptr;
        m_pQueryGrid= nullptr;
    }
    else if(event.m_bOpenDesignForm)
    {
        Close(true); //Close this window.
        GetParent()->ProcessWindowEvent( event );
    }
    else if(event.m_bOpenDesignPage)
    {
        Close(true); //Close this window.
        GetParent()->ProcessWindowEvent( event );
    }
    else if(event.m_bOpenQueryGrid)
        RunQuery(event.m_sTitle, event.m_sQueryToApply);
    else if(event.m_bHelpFrameWasDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }
    else if (event.m_bParseDocument)
        OnParseDocument(event.m_cellValue);

    if(event.m_bRefreshDatabase){

        m_pFormItem= nullptr;

        if(!event.m_sWhereCondition.IsEmpty()){
            SetCurrentStoredWhereCondition(event.m_sWhereCondition);//Store the where condition from the grid in the mainframe.
            SetGridWhereCondition(event.m_sWhereCondition);

            if(m_ComboFilter!=nullptr)
                m_ComboFilter->SetStringSelection("Menu Filter");

            Refresh();
        }
        else if(!event.m_sQueryToApply.IsEmpty()){
            m_sTempQuery=event.m_sQueryToApply;
            Refresh();
        }else{
            //If the events where condition is empty, then see if we have a stored where condition.
            if(event.m_bShowAll){
                SetCurrentStoredWhereCondition("");//Remove the stored where condition because we want to show all records.
            }
            SetGridWhereCondition(GetCurrentStoredWhereCondition());
            if(m_ComboFilter!=nullptr)
                m_ComboFilter->SetStringSelection("Show All");
            Refresh();
        }
    }
}