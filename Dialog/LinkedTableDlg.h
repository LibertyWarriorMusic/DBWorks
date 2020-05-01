//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_LinkedTableDlg_H
#define DBWORKS_LinkedTableDlg_H
#include "DialogBaseClass.h"

class LinkedTableDlg : public DialogBaseClass{
private:
    wxString m_sControlID;

    wxComboBox *pFieldCombo;
    wxArrayString asItemsToDelete;
    void DeleteItems();

    void OnTableComboChange( wxCommandEvent& event );
    void OnFieldComboChange( wxCommandEvent& event );
    wxDECLARE_EVENT_TABLE();
public:
    void SetControlID(wxString id);
    void Save(); // Save the items to the database
    void Load(); // Load the list of items


    LinkedTableDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle);

};
#endif //DBWORKS_LinkedTableDlg_H
