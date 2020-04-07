//
// Created by Nicholas Zounis on 4/4/2020.
//

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Utility.h"
#include "../global.h"


#include "DialogTemplate.h"

wxString DIALOG_TEMPLATE::GetDatabaseToCopy()
{
    //    return m_sSelectedDatabase;
    return "";
}

wxString DIALOG_TEMPLATE::GetNewDatabaseToCopyInto()
{
   // m_sNewDatabaseName =  GetDataValue("ID_SELECT_NAME");
   // m_sNewDatabaseName.Replace(" ",""); //Remove any spaces from the name.
    //return m_sNewDatabaseName;
    return "";
}

DIALOG_TEMPLATE::DIALOG_TEMPLATE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DialogBaseClass( parent, id, title, pos, size, style | wxID_OK )
{
    //m_sSelectedDatabase = "";
    //m_sNewDatabaseName = "";

    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel("Import MySQL Databases.");

    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    // Available combo flags
    // SQL_DATABASES
    // SYS_TABLES
    // READONLY
    // SELECTION_LOOKUP_NAME{TableName;FieldName;}
    // You can use combinations like>  READONLY - SELECTION_LOOKUP_NAME{TableName;FieldName;}
    AddCtlItem(CTL_COMBO_LOOKUP_NAME, "Available Tables *", "Select a table to add to the diagram.", "ID_SELECT_TABLE");
    SetFlags("ID_SELECT_TABLE","SQL_DATABASES");
    wxComboBox *pCom = (wxComboBox*)GetControl("ID_SELECT_TABLE");
    pCom->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( DIALOG_TEMPLATE::OnDatabaseComboChange ), nullptr, this );

    //------------------------------------------------
    // New Database Name

    AddCtlItem(CTL_TEXT, "New Database Name", "Select a name for the new database.", "ID_SELECT_NAME");
   // m_NewDatabaseNameCtl = (wxTextCtrl*)GetControl("ID_SELECT_NAME");

    //------------------------------------------------
    // CREATE TABLES CHECKBOX

    AddCtlItem(CTL_CHECKBOX, "Create Tables", "After the definitions are created, this will create the actual table in the database.", "ID_CHECK_CREATE_TABLE");
    wxCheckBox *pCtlCheck = (wxCheckBox*)GetControl("ID_CHECK_CREATE_TABLE");
    pCtlCheck->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( DIALOG_TEMPLATE::OnCheckCreateTables ), nullptr, this );
    if(Settings.bImportCreateTables)
        SetDataValue("ID_CHECK_CREATE_TABLE","true");

    //------------------------------------------------
    // IMPORT DATA CHECKBOX
    AddCtlItem(CTL_CHECKBOX, "Import Data", "After creating definitions and tables, this will import all the data into the table.", "ID_CHECK_IMPORT_DATA");
    pCtlCheck = (wxCheckBox*)GetControl("ID_CHECK_IMPORT_DATA");
    pCtlCheck->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( DIALOG_TEMPLATE::OnCheckImportData ), nullptr, this );
    if(Settings.bImportData)
        SetDataValue("ID_CHECK_IMPORT_DATA","true");


    //Render all the controls to the mainframe sizer.
    RenderAllControls();

    //Create the buttons to the dialog
    this->Create();

}

void DIALOG_TEMPLATE::OnDatabaseComboChange( wxCommandEvent& event ) {

    // Example
    //wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    //m_sSelectedDatabase = combo->GetStringSelection();
}

void DIALOG_TEMPLATE::OnCheckCreateTables(wxCommandEvent& event)
{
    Settings.bImportCreateTables =GetCheckState("ID_CHECK_CREATE_TABLE");
}

void DIALOG_TEMPLATE::OnCheckImportData(wxCommandEvent& event)
{
    Settings.bImportData = GetCheckState("ID_CHECK_IMPORT_DATA");
}