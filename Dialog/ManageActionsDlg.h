//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_ManageActionsDlg_H
#define DBWORKS_ManageActionsDlg_H
#include "DialogBaseClass.h"

class ManageActionsDlg : public DialogBaseClass{

public:
    ManageActionsDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle);

    void SetControlID(wxString id);
    wxString GetAction();
    void Save(); // Save the items to the database
    void Load(); // Load the list of items
private:
    wxString m_sControlID;
};
#endif //DBWORKS_ManageActionsDlg_H
