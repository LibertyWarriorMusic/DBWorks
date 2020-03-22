

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


#include "global.h"
#include "HtmlHelp.h"


//Add my own classes here
#include "GenericItemForm.h"
#include "MyEvent.h"
#include "DBGrid.h"
#include "ImagePanel.h"
#include "GenericTable.h"

#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(ArrayTableField);
#include "Utility.h"

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
}


void GenericTable::AddField(const wxString& title, const wxString& field, const wxString& type, const wxString& flag, const wxString& defaultVal, const wxString& KeyVal, const wxString& ExtraVal)
{
    
    auto * tableField = new TableField();
    tableField->fieldName=field;
    tableField->title=title;
    tableField->type = type;
    tableField->flag = flag;
    tableField->extra = ExtraVal;
    tableField->key = KeyVal;
    tableField->defaultValue = defaultVal;
    
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
                         m_Grid->AddItem(m_FieldArray[index].title,m_FieldArray[index].fieldName,m_FieldArray[index].flag,m_FieldArray[index].defaultValue);
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
            return false;
        }

        wxInitAllImageHandlers(); //You need to call this or the images will not load.
        // Get the path to the images
        wxString strExe = wxStandardPaths::Get().GetExecutablePath();
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


        m_Toolbar->Realize();
        SetToolBar(m_Toolbar);
        SetSize((int)Settings.lMainWindowWidth,(int)Settings.lMainWindowHeight);

        this->SetBackgroundColour(wxColour(77,120,77));
        //This is here for debugging.
        //SetStatusText("Message = " + Settings.Message);
    
    return true;
    
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

    formItem = new GenericItemForm((wxFrame*) this, -1,"Add Item",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP| (unsigned)wxRESIZE_BORDER);
    
    
    int count = m_FieldArray.GetCount();

    if (count>0){
        
        for(int index=0;index<count;index++)
            formItem->AddItem(m_FieldArray[index].title,m_FieldArray[index].fieldName,m_FieldArray[index].flag,m_FieldArray[index].type,m_FieldArray[index].defaultValue);
    }

    //Note: this has to come before CreateField because m_sTableName is referenced in CreateFields() function
    formItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName,m_sTableName+"Id");
   
    formItem->SetUse("ADD");
    formItem->CreateFields();
    

    
    formItem->Show(true);
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
    formItem = new GenericItemForm((wxFrame*) this, -1,"Edit Item",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP | (unsigned)wxRESIZE_BORDER);
    int count = m_FieldArray.GetCount();

    if (count>0){

        for(int index=0;index<count;index++)
            formItem->AddItem(m_FieldArray[index].title,m_FieldArray[index].fieldName,m_FieldArray[index].flag,m_FieldArray[index].type,m_FieldArray[index].defaultValue);
    }


    formItem->SetUse("UPDATE");

    formItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName,m_sTableName+"Id");
    formItem->CreateFields();


    wxString spasswordId = m_Grid->GetCellValue(rowID,0);
    SetStatusText(spasswordId);

    formItem->SetID(spasswordId);
    formItem->Show(true);
    formItem->LoadFields();
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
    if(formItem != nullptr){
        //formItem->Destroy();
        formItem = nullptr;
    }

    formItem = new GenericItemForm((wxFrame*) this, -1,"View Item",wxDefaultPosition,wxDefaultSize,(unsigned)wxCAPTION | (unsigned)wxSTAY_ON_TOP | (unsigned)wxRESIZE_BORDER);

    if(formItem != nullptr){
        //Define the database
        int count = m_FieldArray.GetCount();

        if (count>0){

            for(int index=0;index<count;index++)
                formItem->AddItem(m_FieldArray[index].title,m_FieldArray[index].fieldName,m_FieldArray[index].flag,m_FieldArray[index].type,m_FieldArray[index].defaultValue);
        }

        formItem->SetUse("VIEW");
        formItem->SetSettings(Settings.sDatabase,Settings.sServer,Settings.sDatabaseUser,Settings.sPassword,m_sTableName,m_sTableName+"Id");
        formItem->CreateFields();




        wxString spasswordId = m_Grid->GetCellValue((int)rowID,0);
        SetStatusText(spasswordId);

        formItem->SetID(spasswordId);
        formItem->Show(true);
        formItem->LoadFields();
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

void GenericTable::SetTableDefinition(const wxString& tableName, const wxString& title, const wxString& comments, const wxString& whereCondition)
{
    m_sTableName = tableName;
    m_sTitle=title;
    m_sComments=comments;
    m_sWhereCondition=whereCondition;

}



//We created an event to refresh the grid so we can call it from any frame class.
void GenericTable::OnMyEvent(MyEvent& event )
{
   // wxString msg;
   // msg << event.GetEventType();
       // wxMessageBox( msg);
    //NOTE: We get here if resizing or right menu clicking, so we need to create a flag to indicated which.
    if(event.m_bProperties)
         wxMessageBox("We are in Table from properties.");
    else if(event.m_bOpen)
        ViewItem(event.m_iRow);
    else if (event.m_bEdit)
        EditItem(event.m_iRow);
    else if(event.m_bDestroyed){
        m_HtmlWin = nullptr; // This allows us to test the help window if it was destroyed internally, like when you press the close icon in the window. See OnBHelp below.
    }
    else if (event.m_bParseDocument)
        OnParseDocument(event.m_cellValue);
    //wxMessageBox( _("Looks like it works!"), _("MyEvent reached"),
     //   wxOK | wxICON_INFORMATION, this );
    //SetStatusText("Event Worked");
    if(event.m_bRefreshDatabase){
        SetGridWhereCondition(event.m_sWhereCondition);
        m_Grid->LoadGridFromDatabase();
        this->Layout(); // This is the key, you need to relayout the form.
    }
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
   // m_Grid->SetGridWhereCondition(whereToBlend); // Make sure you show all records.



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




//We can send a message to the parent that this window is destroyed.
bool GenericTable::Destroy()
{

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    bool bResult = wxFrame::Destroy();
    return bResult;
}


