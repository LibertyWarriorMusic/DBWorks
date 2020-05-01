//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_SelectionFieldQueryDlg_H
#define DBWORKS_SelectionFieldQueryDlg_H
#include "DialogBaseClass.h"

class SelectionFieldQueryDlg : public DialogBaseClass{

public:
    SelectionFieldQueryDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle);
    void SetControlID(wxString id);
    void SetQuery(wxString sQuery);
    wxString GetField();
    void Save(); // Save the items to the database
    void Load(); // Load the list of items
private:
    wxString m_sControlID;
    wxString m_sQuery;
};
#endif //DBWORKS_SelectionFieldQueryDlg_H
