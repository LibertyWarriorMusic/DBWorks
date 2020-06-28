//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_ManageActionsDlg_H
#define DBWORKS_ManageActionsDlg_H
#include "DialogBaseClass.h"

class ManageActionsDlg : public DialogBaseClass{

public:
    ManageActionsDlg(wxWindow* parent, wxString id, wxString sDialogTitle, wxString sFieldTitle);
    wxString GetAction();
    void Save(); // Save the items to the database

private:
    wxString m_sControlID;
    wxString m_sSelectedAction;
    void CheckActionChange(wxString sAction);
    void OnActionComboChange( wxCommandEvent& event );
};
#endif //DBWORKS_ManageActionsDlg_H
