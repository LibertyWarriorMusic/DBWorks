//
// Created by Nicholas Zounis on 23/3/20.
//

#define BORDERWIDTH 3
#define TEXTWIDTH 140
#define ALLOW_TO_GROW 1

#include <wx/wxprec.h>
#include<wx/wx.h>

#include "MyEvent.h"
#include "Utility.h"
#include "global.h"

#include "ImportMySQLDatabase.h"

ImportMySQLDatabase::ImportMySQLDatabase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    m_sSelectedDatabase = "";
    m_sNewDatabaseName = "";
    m_CreateTablesCtl = nullptr;
    m_ImportDataCtl =nullptr;

    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

    this->SetLabel("Import MySQL Databases.");

    //------------------------------------------------
    // AVAILABLE DATABASES COMBO
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText* txt = new wxStaticText( this, wxID_ANY, Settings.sUsergroup, wxDefaultPosition, wxSize(TEXTWIDTH,20), 0 );
    txt->SetLabel("Available Databases *");
    sizer1->Add( txt, 0, wxALL, BORDERWIDTH );

    m_AvailableDatabasesCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,wxCB_READONLY);
    m_AvailableDatabasesCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( ImportMySQLDatabase::OnDatabaseComboChange ), nullptr, this );
    m_AvailableDatabasesCombo->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( ImportMySQLDatabase::OnDatabaseComboDropDown ), nullptr, this );
    sizer1->Add( m_AvailableDatabasesCombo, ALLOW_TO_GROW, wxALL, BORDERWIDTH );

    wxArrayString sSelectionItemArray;
    Utility::LoadStringArrayWidthMySQLDatabaseNames(sSelectionItemArray);

    //Fill the list box with the selection items.
    for ( int index=0; index<sSelectionItemArray.GetCount(); index++ )
        m_AvailableDatabasesCombo->Append(sSelectionItemArray[index]);

    m_MainFormSizer->Add( sizer1, 0, wxEXPAND, BORDERWIDTH );

    //------------------------------------------------
    // New Database Name
    sizer1 = new wxBoxSizer( wxHORIZONTAL );

    titleNewDB = new wxStaticText( this, wxID_ANY, "New Database Name", wxDefaultPosition, wxSize(TEXTWIDTH,20), 0 );
    sizer1->Add( titleNewDB, 0, wxALL, BORDERWIDTH );

    m_NewDatabaseNameCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    sizer1->Add( m_NewDatabaseNameCtl, ALLOW_TO_GROW, wxEXPAND, BORDERWIDTH );


    m_MainFormSizer->Add( sizer1, 0, wxEXPAND, BORDERWIDTH);
    //------------------------------------------------
    // CREATE TABLES CHECKBOX
    sizer1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText* lb = new wxStaticText( this, wxID_ANY, "", wxDefaultPosition, wxSize(TEXTWIDTH,20), 0 );
    lb->SetLabel("Create Tables");
    sizer1->Add( lb, 0, wxALL, BORDERWIDTH );

    m_CreateTablesCtl = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_CreateTablesCtl->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( ImportMySQLDatabase::OnCheckCreateTables ), nullptr, this );
    sizer1->Add( m_CreateTablesCtl, 0, wxALL, BORDERWIDTH);

    m_MainFormSizer->Add( sizer1, 0, wxEXPAND, BORDERWIDTH);

    //Default check value
    if(Settings.bImportCreateTables)
        m_CreateTablesCtl->SetValue(true);

    //------------------------------------------------
    // IMPORT DATA CHECKBOX
    sizer1 = new wxBoxSizer( wxHORIZONTAL );

    lb = new wxStaticText( this, wxID_ANY, "", wxDefaultPosition, wxSize(TEXTWIDTH,20), 0 );
    lb->SetLabel("Import Data");
    sizer1->Add( lb, 0, wxALL, BORDERWIDTH );

    m_ImportDataCtl = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_ImportDataCtl->Connect( wxEVT_CHECKBOX, wxCommandEventHandler( ImportMySQLDatabase::OnCheckImportData ), nullptr, this );
    sizer1->Add( m_ImportDataCtl, 0, wxALL, BORDERWIDTH );
    m_MainFormSizer->Add( sizer1, 0, wxEXPAND, BORDERWIDTH );

    //Default check value
    if(Settings.bImportData)
        m_ImportDataCtl->SetValue(true);

    //------------------------------------------------
    // BUTTONS IMPORT AND CANCEL

    wxBoxSizer *btSizer = new wxBoxSizer( wxHORIZONTAL );

    m_Import = new wxButton( this, wxID_ANY , wxT("Import Database"), wxDefaultPosition, wxSize(120,20), 0 );
    //m_Import->SetMaxSize( wxSize( 120,20 ) );
    m_Import->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportMySQLDatabase::OnbImport ), nullptr, this );
    btSizer->Add( m_Import, 0, wxALL, BORDERWIDTH );


    m_Cancel = new wxButton( this, wxID_ANY , wxT("Cancel"), wxDefaultPosition, wxSize(120,20), 0 );
    //m_Cancel->SetMaxSize( wxSize( 120,20 ) );
    m_Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportMySQLDatabase::OnbCancel ), nullptr, this );
    btSizer->Add( m_Cancel, 0, wxALL, BORDERWIDTH );



    m_MainFormSizer->Add( btSizer, 0, wxALIGN_CENTER, BORDERWIDTH);
    this->SetSizer( m_MainFormSizer );
    this->Layout();


}

ImportMySQLDatabase::~ImportMySQLDatabase(){

}

void ImportMySQLDatabase::OnDatabaseComboChange( wxCommandEvent& event ) {

    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    m_sSelectedDatabase = combo->GetStringSelection();
}

void ImportMySQLDatabase::OnDatabaseComboDropDown( wxCommandEvent& event )
{

}

//We can send a message to the parent that this window is destroyed.
bool ImportMySQLDatabase::Destroy()
{
    bool bResult = wxFrame::Destroy();

    MyEvent my_event( this );
    my_event.m_bDestroyed=true;
    GetParent()->ProcessWindowEvent( my_event );

    return bResult;
}

void ImportMySQLDatabase::OnCheckCreateTables(wxCommandEvent& event)
{
    Settings.bImportCreateTables = m_CreateTablesCtl->GetValue();
}

void ImportMySQLDatabase::OnCheckImportData(wxCommandEvent& event)
{
    Settings.bImportData = m_ImportDataCtl->GetValue();
}

void ImportMySQLDatabase::OnbCancel( wxCommandEvent& event )
{
    //Refresh the control by generating an event to the main frame.
    MyEvent my_event( this );
    GetParent()->ProcessWindowEvent( my_event ) ;

    Close(TRUE);
}

void ImportMySQLDatabase::OnbImport( wxCommandEvent& event )
{
    m_sNewDatabaseName =  m_NewDatabaseNameCtl->GetValue();
    m_sNewDatabaseName.Replace(" ",""); //Remove any spaces from the name.

    Close(TRUE);
    //Refresh the control by generating an event to the main frame.
    MyEvent my_event( this );
    my_event.m_sDatabaseName = m_sSelectedDatabase;
    my_event.m_sNewDatabaseName = m_sNewDatabaseName;
    my_event.m_bImportDatabase = true;
    GetParent()->ProcessWindowEvent( my_event ) ;


}