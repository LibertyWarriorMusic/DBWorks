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

#include "LinkedTableDlg.h"

enum {
    ID_BUTTON_CLICKED= wxID_HIGHEST + 1600,
    ID_BUTTON_DELETE
};


wxBEGIN_EVENT_TABLE(LinkedTableDlg, DialogBaseClass)
                //EVT_BUTTON(ID_BUTTON_CLICKED, LinkedTableDlg::OnClickButton)
               // EVT_BUTTON(ID_BUTTON_DELETE, LinkedTableDlg::OnClickDeleteButton)
wxEND_EVENT_TABLE()

LinkedTableDlg::LinkedTableDlg(wxWindow* parent, wxString sDialogTitle, wxString sFieldTitle) : DialogBaseClass( parent, wxID_ANY, sDialogTitle, wxDefaultPosition, wxSize( 1000,900 ), wxDEFAULT_FRAME_STYLE | wxID_OK | wxSTAY_ON_TOP)
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );

    this->SetLabel(sDialogTitle);

    AddCtlItem(CTL_SELECTION_LOOKUP_NAME, "Linked Table", "Select a table to link.", "ID_SELECT_TABLE");
    SetFlags("ID_SELECT_TABLE","SYS_TABLES");
    wxComboBox *pCom = (wxComboBox*)GetControl("ID_SELECT_TABLE");
    pCom->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( LinkedTableDlg::OnTableComboChange ), nullptr, this );

    AddCtlItem(CTL_SELECTION_LOOKUP_NAME, "Display Field", "Select a field to display.", "ID_SELECT_FIELD");
    //SetFlags("ID_SELECT_FIELD","SYS_TABLES");
    pFieldCombo = (wxComboBox*)GetControl("ID_SELECT_FIELD");
    pFieldCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( LinkedTableDlg::OnFieldComboChange ), nullptr, this );

    RenderAllControls();
    SetSize(wxSize(600,200));
}

void LinkedTableDlg::OnTableComboChange( wxCommandEvent& event ) {

    wxComboBox * combo = (wxComboBox*) event.GetEventObject();
    wxString sTableName = combo->GetStringSelection();

    if(!sTableName.IsEmpty()){
        pFieldCombo->Clear();
        wxArrayString asFieldNames;
        Utility::LoadStringArrayWithFieldNamesFromQuery(asFieldNames,"SELECT * FROM "+sTableName);
        Utility::LoadComboFromStringArray(pFieldCombo,asFieldNames);
    }
}
void LinkedTableDlg::OnFieldComboChange( wxCommandEvent& event ) {

    wxComboBox * combo = (wxComboBox*) event.GetEventObject();

}
void LinkedTableDlg::SetControlID(wxString id)
{
    m_sControlID=id;
}

void LinkedTableDlg::DeleteItems()
{
    for(int index=0;index<asItemsToDelete.GetCount();index++)
            Utility::ExecuteQuery(asItemsToDelete[index]);
}

void LinkedTableDlg::Load()
{
    wxString sValue="";
    Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Linked_Table",sValue);
    SetDataValue("ID_SELECT_TABLE",sValue);

    Utility::LoadTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Display_Field",sValue);
    SetDataValue("ID_SELECT_FIELD",sValue);

}

void LinkedTableDlg::Save()
{
    wxString sValue = GetDataValue("ID_SELECT_TABLE");
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Linked_Table",sValue);

    sValue = GetDataValue("ID_SELECT_FIELD");
    Utility::SaveTableData(Settings.sDatabase,"usr_controls",m_sControlID,"Display_Field",sValue);
}