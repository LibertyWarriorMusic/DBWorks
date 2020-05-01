//
// Created by Nicholas Zounis on 23/3/20.
//

#ifndef DBWORKS_ListBoxDlg_H
#define DBWORKS_ListBoxDlg_H
#include "DialogBaseClass.h"

class ListBoxDlg : public DialogBaseClass{
private:
    wxString m_sControlID;

    wxListBox *pListBox;
    wxArrayString asItemsToDelete;
    void DeleteItems();

    void OnClickDeleteButton( wxCommandEvent& event );
    void OnClickButton( wxCommandEvent& event );

    wxDECLARE_EVENT_TABLE();
public:
    void SetControlID(wxString id);
    void SaveItems(); // Save the items to the database
    void LoadItems(); // Load the list of items


    ListBoxDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle);

};
#endif //DBWORKS_ListBoxDlg_H
