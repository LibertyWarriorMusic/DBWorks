//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_IMPORTMYSQLDATABASE_H
#define DBWORKS_IMPORTMYSQLDATABASE_H
#include "DialogBaseClass.h"


class ImportMySQLDatabase : public DialogBaseClass{

private:
   // wxComboBox * m_AvailableDatabasesCombo;
    wxString m_sSelectedDatabase;
    wxString m_sNewDatabaseName;

   // wxStaticText* titleNewDB;
   // wxCheckBox * m_CreateTablesCtl;
   // wxCheckBox * m_ImportDataCtl;
public:
    explicit ImportMySQLDatabase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);

    wxString GetDatabaseToCopy();
    wxString GetNewDatabaseToCopyInto();

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.

    void OnDatabaseComboChange(wxCommandEvent& event);

    void OnCheckCreateTables(wxCommandEvent& event);
    void OnCheckImportData(wxCommandEvent& event);
};
#endif //DBWORKS_IMPORTMYSQLDATABASE_H
