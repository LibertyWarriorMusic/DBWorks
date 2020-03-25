//
// Created by Nicholas Zounis on 23/3/20.
//



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

    m_MainFormSizer = new wxBoxSizer( wxVERTICAL );

    m_txtCltUserGroup = new wxStaticText( this, wxID_ANY, Settings.sUsergroup, wxDefaultPosition, wxDefaultSize, 0 );
    m_txtCltUserGroup->SetLabel("Select a MySQL Databases on your system to import.");

    m_UserGroupCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,0,wxCB_READONLY);
    m_UserGroupCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( ImportMySQLDatabase::OnDatabaseComboChange ), nullptr, this );
    m_UserGroupCombo->Connect( wxEVT_COMBOBOX_DROPDOWN, wxCommandEventHandler( ImportMySQLDatabase::OnDatabaseComboDropDown ), nullptr, this );
    m_UserGroupCombo->SetSize(260,-1);

    wxArrayString sSelectionItemArray;
    Utility::LoadStringArrayWidthMySQLDatabaseNames(sSelectionItemArray);

    //Fill the list box with the selection items.
    for ( int index=0; index<sSelectionItemArray.GetCount(); index++ )
        m_UserGroupCombo->Append(sSelectionItemArray[index]);


    m_MainFormSizer->Add( m_txtCltUserGroup, 0 , wxEXPAND, 5);
    m_MainFormSizer->Add( m_UserGroupCombo, 0 , wxEXPAND, 5);

    titleNewDB = new wxStaticText( this, wxID_ANY, "New Database Name", wxDefaultPosition, wxDefaultSize, 0 );
   // titleNewDB->SetLabel("Select a MySQL Databases on your system to import.");


    m_NewDatabaseNameCtl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_MainFormSizer->Add( titleNewDB, 0 , wxEXPAND, 5);
    m_MainFormSizer->Add( m_NewDatabaseNameCtl,0, wxEXPAND, 5 );


    wxBoxSizer *btSizer = new wxBoxSizer( wxHORIZONTAL );

    m_Import = new wxButton( this, wxID_ANY , wxT("Import Database"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Import->SetMaxSize( wxSize( 120,20 ) );
    m_Import->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportMySQLDatabase::OnbImport ), nullptr, this );
    btSizer->Add( m_Import, 0, wxALL, 5 );


    m_Cancel = new wxButton( this, wxID_ANY , wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Cancel->SetMaxSize( wxSize( 120,20 ) );
    m_Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImportMySQLDatabase::OnbCancel ), nullptr, this );
    btSizer->Add( m_Cancel, 0, wxALL, 5 );



    m_MainFormSizer->Add( btSizer, 0, wxEXPAND, 1 );

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