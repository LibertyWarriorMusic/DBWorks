//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_IMPORTMYSQLDATABASE_H
#define DBWORKS_IMPORTMYSQLDATABASE_H


class ImportMySQLDatabase : public wxFrame{

private:
    wxBoxSizer* m_MainFormSizer;
    wxStaticText* m_txtCltUserGroup;
    wxComboBox * m_UserGroupCombo;
    wxButton* m_Cancel;
    wxButton* m_Import;
    wxString m_sSelectedDatabase;
    wxString m_sNewDatabaseName;
    wxTextCtrl * m_NewDatabaseNameCtl;
    wxStaticText* titleNewDB;
public:
    explicit ImportMySQLDatabase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,900 ), long style = wxDEFAULT_FRAME_STYLE);

    ~ImportMySQLDatabase() override;

    //We are overriding the Destroy function so we can send a message to the parent that this window has been destroyed.
    // This is needed if the parent can null any pointers to this class so they can be tested for null condition.
    bool Destroy() override;
    void OnDatabaseComboChange(wxCommandEvent& event);
    void OnDatabaseComboDropDown( wxCommandEvent& event );
    void OnbCancel( wxCommandEvent& event );
    void OnbImport( wxCommandEvent& event );
};
#endif //DBWORKS_IMPORTMYSQLDATABASE_H
