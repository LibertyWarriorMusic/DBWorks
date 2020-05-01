//
// Created by Nicholas Zounis on 23/3/20.
//

#define BORDERWIDTH 3
#define TEXTWIDTH 140
#define ALLOW_TO_GROW 1

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "../MyEvent.h"
#include "../Shared/Utility.h"
#include "../Shared/global.h"


#include "ImportMySQLDatabase.h"

wxString ImportMySQLDatabase::GetDatabaseToCopy()
{
    return m_sSelectedDatabase;

}

wxString ImportMySQLDatabase::GetNewDatabaseToCopyInto()
{
    m_sNewDatabaseName =  GetDataValue("ID_SELECT_NAME");
    m_sNewDatabaseName.Replace(" ",""); //Remove any spaces from the name.
    return m_sNewDatabaseName;
}

ImportMySQLDatabase::ImportMySQLDatabase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DialogBaseClass( parent, id, title, pos, size, style | wxID_OK )
{
    m_sSelectedDatabase = "";
    m_sNewDatabaseName = "";
//    m_CreateTablesCtl = nullptr;
  //  m_ImportDataCtl =nullptr;
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel("Import MySQL Databases.");

    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    AddCtlItem(CTL_SELECTION_LOOKUP_NAME, "Available Databases", "Select a table to add to the diagram.", "ID_SELECT_TABLE");
    SetFlags("ID_SELECT_TABLE","SQL_DATABASES");
    wxComboBox *pCom = (wxComboBox*)GetControl("ID_SELECT_TABLE");
    pCom->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( ImportMySQLDatabase::OnDatabaseComboChange ), nullptr, this );

    //------------------------------------------------
    // New Database Name

    AddCtlItem(CTL_TEXT, "New Database Name", "Select a name for the new database.", "ID_SELECT_NAME");
   // m_NewDatabaseNameCtl = (wxTextCtrl*)GetControl("ID_SELECT_NAME");

    //------------------------------------------------
    // CREATE TABLES CHECKBOX

    AddCtlItem(CTL_CHECKBOX, "Create Tables", "After the definitions are created, this will create the actual table in the database.", "ID_CHECK_CREATE_TABLE");
    wxCheckBox *pCtlCheck = (wxCheckBox*)GetControl("ID_CHECK_CREATE_TABLE");
    pCtlCheck->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( ImportMySQLDatabase::OnCheckCreateTables ), nullptr, this );
    if(Settings.bImportCreateTables)
        SetDataValue("ID_CHECK_CREATE_TABLE","true");

    //------------------------------------------------
    // IMPORT DATA CHECKBOX
    AddCtlItem(CTL_CHECKBOX, "Import Data", "After creating definitions and tables, this will import all the data into the table.", "ID_CHECK_IMPORT_DATA");
    pCtlCheck = (wxCheckBox*)GetControl("ID_CHECK_IMPORT_DATA");
    pCtlCheck->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( ImportMySQLDatabase::OnCheckImportData ), nullptr, this );
    if(Settings.bImportData)
        SetDataValue("ID_CHECK_IMPORT_DATA","true");


    //Render all the controls to the mainframe sizer.
    RenderAllControls();

}

void ImportMySQLDatabase::OnDatabaseComboChange( wxCommandEvent& event ) {

    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    m_sSelectedDatabase = combo->GetStringSelection();
}

void ImportMySQLDatabase::OnCheckCreateTables(wxCommandEvent& event)
{
    Settings.bImportCreateTables =GetCheckState("ID_CHECK_CREATE_TABLE");
}

void ImportMySQLDatabase::OnCheckImportData(wxCommandEvent& event)
{
    Settings.bImportData = GetCheckState("ID_CHECK_IMPORT_DATA");
}